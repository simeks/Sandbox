// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "D3D11DeviceContext.h"
#include "D3D11ResourceManager.h"
#include "D3D11Buffer.h"
#include "D3D11Shader.h"

#include <Engine/Rendering/RVertexBuffer.h>
#include <Engine/Rendering/RIndexBuffer.h>
#include <Engine/Rendering/RVertexDeclaration.h>
#include <Foundation/Profiler/Profiler.h>


namespace sb
{

	//-------------------------------------------------------------------------------
	D3D11DeviceContext::D3D11DeviceContext(ID3D11DeviceContext* d3d_context,
		D3D11RenderDevice* device, D3D11ResourceManager* resource_manager)
		: _context(d3d_context),
		_device(device),
		_resource_manager(resource_manager),
		_output_merger_stage(this),
		_input_assembler_stage(this),
		_rasterizer_stage(this),
		_shader_stage(this),
		_instance_merger(_device)
	{
	}
	D3D11DeviceContext::~D3D11DeviceContext()
	{
	}
	//-------------------------------------------------------------------------------
	void D3D11DeviceContext::Dispatch(uint32_t count, const RenderContext::SortCmd* commands)
	{
		PROFILER_SCOPE("D3D11DeviceContext::Dispatch");

		for (uint32_t i = 0; i < count; ++i)
		{
			const RenderContext::SortCmd& command = commands[i];

			// Create a static memory stream for the command
			StaticMemoryStream stream(memory::PointerAdd(command.buffer->data(), command.offset), command.length);

			uint8_t cmd;
			stream.Read(&cmd, 1);

			// Dispatch any draw calls we have in the instance merger if we're getting a state command.
			if (_instance_merger.Size() && cmd != RenderContext::RC_DRAW)
			{
				_instance_merger.Dispatch(this);
			}

			switch (cmd)
			{
			case RenderContext::RC_SET_TARGETS:
			{
				RenderContext::SetTargetsCmd* cmd_data = (RenderContext::SetTargetsCmd*)stream.Current();

				for (uint32_t i = 0; i < MAX_MULTIPLE_RENDER_TARGETS; ++i)
				{
					_output_merger_stage.SetRenderTarget(i, cmd_data->render_target[i]);
				}
				_output_merger_stage.SetDepthStencilTarget(cmd_data->depth_target);
				_rasterizer_stage.SetViewports(cmd_data->num_viewports, (D3D11_VIEWPORT*)memory::PointerAdd(cmd_data, cmd_data->viewport_offset));

				ScissorRect* rect = (ScissorRect*)memory::PointerAdd(stream.Current(), cmd_data->scissor_rect_offset);
				D3D11_RECT d3drect[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
				for (uint32_t r = 0; r < cmd_data->num_scissor_rects; ++r)
				{
					d3drect[r].left = (LONG)rect[r].x;
					d3drect[r].top = (LONG)rect[r].y;
					d3drect[r].right = (LONG)rect[r].x + (LONG)rect[r].width;
					d3drect[r].bottom = (LONG)rect[r].y + (LONG)rect[r].height;
				}
				_rasterizer_stage.SetScissorRects(cmd_data->num_scissor_rects, d3drect);

				break;
			}

			case RenderContext::RC_CLEAR_STATE:
			{
				ClearState();
				break;
			}

			case RenderContext::RC_CLEAR_TARGETS:
			{
				RenderContext::ClearTargetsCmd* cmd_data = (RenderContext::ClearTargetsCmd*)stream.Current();

				// Clear bound targets
				ID3D11RenderTargetView** render_target_view = _output_merger_stage.GetRenderTargetViews();
				ID3D11DepthStencilView* depth_stencil_view = _output_merger_stage.GetDepthStencilView();
				if (cmd_data->flags & rendering::CLEAR_COLOR)
				{
					for (uint32_t rt = 0; rt < MAX_MULTIPLE_RENDER_TARGETS; ++rt)
					{
						if (render_target_view[rt])
						{
							_context->ClearRenderTargetView(render_target_view[rt], cmd_data->color);
						}
					}
				}
				if (depth_stencil_view)
				{
					UINT clear_flags = 0;
					if (cmd_data->flags & rendering::CLEAR_DEPTH)
						clear_flags |= D3D11_CLEAR_DEPTH;
					if (cmd_data->flags & rendering::CLEAR_STENCIL)
						clear_flags |= D3D11_CLEAR_STENCIL;

					_context->ClearDepthStencilView(depth_stencil_view, clear_flags, cmd_data->depth, cmd_data->stencil);
				}
				break;
			}
			case RenderContext::RC_DRAW:
			{
				RenderContext::DrawCmd* cmd_data = (RenderContext::DrawCmd*)stream.Current();
				_instance_merger.Add(cmd_data, command.sort_key);
				break;
			}
			case RenderContext::RC_DISPATCH:
			{
				RenderContext::DispatchCmd* cmd_data = (RenderContext::DispatchCmd*)stream.Current();
				DispatchCompute(cmd_data, command.sort_key);
				break;
			}
			case RenderContext::RC_UPDATE_BUFFER:
			{
				RenderContext::UpdateBufferCmd* cmd_data = (RenderContext::UpdateBufferCmd*)stream.Current();

				void* data = (cmd_data->data_offset != 0) ? memory::PointerAdd(cmd_data, cmd_data->data_offset) : NULL;

				_resource_manager->UpdateResource(this, &cmd_data->resource, data);
				break;
			}
			};

		}
		// Dispatch any remaining draw calls
		if (_instance_merger.Size())
		{
			_instance_merger.Dispatch(this);
		}
	}
	//-------------------------------------------------------------------------------
	void D3D11DeviceContext::Draw(RenderContext::DrawCmd* cmd, uint64_t sort_key)
	{
		bool indexed = false;

		// Bind shader and resources
		RenderResource* resources = (RenderResource*)memory::PointerAdd(cmd, cmd->shader_resources_offset);

		Assert(IsValid(cmd->shader));
		D3D11Shader* shader = _resource_manager->GetShader(cmd->shader);
		_shader_stage.BindShader(shader, sort_key);
		_shader_stage.BindResources(resources, cmd->shader_resources_count);

		D3D11ShaderPass* shader_pass = _shader_stage.GetShaderPass();
		Assert(shader_pass);

		// Bind constant buffers and update if necessary 
		RConstantBuffer* cb = (RConstantBuffer*)memory::PointerAdd(cmd, cmd->constant_buffer_offset);
		_shader_stage.BindConstantBuffers(cb, cmd->constant_buffer_count, memory::PointerAdd(cmd, cmd->constant_data_offset));

		_input_assembler_stage.SetVertexProgram(shader_pass->GetVertexShader());
		_input_assembler_stage.SetPrimitiveType(cmd->draw_call.prim_type);


		// Bind input-assembler resources
		RenderResource* ia_resources = (RenderResource*)memory::PointerAdd(cmd, cmd->ia_resource_offset);
		for (uint32_t i = 0; i < cmd->ia_resource_count; ++i)
		{
			switch (ia_resources[i].GetType())
			{
			case RenderResource::VERTEX_BUFFER:
			{
				// TODO: Specifying vertex buffer slot
				_input_assembler_stage.SetVertexBuffers(0, &ia_resources[i], cmd->draw_call.vertex_offset);
				break;
			}
			case RenderResource::INDEX_BUFFER:
			{
				if (IsValid(ia_resources[i].GetHandle()))
					indexed = true;

				_input_assembler_stage.SetIndexBuffer(&ia_resources[i], 0);
				break;
			}
			case RenderResource::VERTEX_DECLARATION:
			{
				_input_assembler_stage.SetVertexDeclaration(&ia_resources[i]);
				break;
			}
			default:
				Assert(false);
				logging::Error("Invalid resource type");
				break;
			};
		}

		// Bind render states
		_output_merger_stage.SetBlendState(shader_pass->GetBlendState());
		_output_merger_stage.SetDepthStencilState(shader_pass->GetDepthStencilState());
		// Rasterizer state
		_rasterizer_stage.SetRasterizerState(shader_pass->GetRasterizerState());


		if (_input_assembler_stage.Dirty())
			_input_assembler_stage.Apply();
		if (_output_merger_stage.Dirty())
			_output_merger_stage.ApplyPixelStage();
		if (_rasterizer_stage.Dirty())
			_rasterizer_stage.Apply();

		if (cmd->draw_call.instance_count != 0)
		{
			if (indexed)
				_context->DrawIndexedInstanced(cmd->draw_call.index_count, cmd->draw_call.instance_count, cmd->draw_call.index_offset, 0, 0);
			else
				_context->DrawInstanced(cmd->draw_call.vertex_count, cmd->draw_call.instance_count, 0, 0);
		}
		else
		{
			if (indexed)
				_context->DrawIndexed(cmd->draw_call.index_count, cmd->draw_call.index_offset, 0);
			else
				_context->Draw(cmd->draw_call.vertex_count, 0);

		}
	}

	void D3D11DeviceContext::DispatchCompute(RenderContext::DispatchCmd* cmd, uint64_t sort_key)
	{
		// Bind shader and resources
		RenderResource* resources = (RenderResource*)memory::PointerAdd(cmd, cmd->shader_resources_offset);

		Assert(IsValid(cmd->shader));
		D3D11Shader* shader = _resource_manager->GetShader(cmd->shader);
		_shader_stage.BindShader(shader, sort_key);
		_shader_stage.BindResources(resources, cmd->shader_resources_count);

		D3D11ShaderPass* shader_pass = _shader_stage.GetShaderPass();
		Assert(shader_pass);
		Assert(shader_pass->GetComputeShader());

		_output_merger_stage.BindUnorderedAccessViews(resources, cmd->shader_resources_count, shader_pass);
		_output_merger_stage.ApplyComputeStage();

		// Bind constant buffers and update if necessary 
		RConstantBuffer* cb = (RConstantBuffer*)memory::PointerAdd(cmd, cmd->constant_buffer_offset);
		_shader_stage.BindConstantBuffers(cb, cmd->constant_buffer_count, memory::PointerAdd(cmd, cmd->constant_data_offset));

		_context->Dispatch(cmd->thread_group_count_x, cmd->thread_group_count_y, cmd->thread_group_count_z);

	}

	//-------------------------------------------------------------------------------
	D3D11ResourceManager* D3D11DeviceContext::GetResourceManager() const
	{
		return _resource_manager;
	}
	ID3D11DeviceContext* D3D11DeviceContext::GetD3DContext() const
	{
		Assert(_context);
		return _context;
	}

	//-------------------------------------------------------------------------------
	void D3D11DeviceContext::ClearState()
	{
		// Clear our stages
		_output_merger_stage.Clear();
		_input_assembler_stage.Clear();
		_rasterizer_stage.Clear();
		_shader_stage.Clear();

		// Clears the current d3d11 states
		_context->ClearState();
	}

	//-------------------------------------------------------------------------------

} // namespace sb

