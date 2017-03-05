// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "RenderContext.h"
#include "RShader.h"
#include "RConstantBuffer.h"
#include "RRenderTarget.h"

#include <Foundation/IO/MemoryStream.h>

namespace sb
{

	RenderContext::Targets::Targets()
	{
		for (uint32_t i = 0; i < MAX_MULTIPLE_RENDER_TARGETS; ++i)
		{
			render_targets[i] = RRenderTarget();
		}
		depth_stencil_target = RRenderTarget();

		num_viewports = 0;
		viewports = nullptr;

		num_scissor_rects = 0;
		rects = nullptr;
	}

	//-------------------------------------------------------------------------------
	RenderContext::RenderContext()
		: _data_stream(&_cmd_buffer)
	{

	}
	RenderContext::~RenderContext()
	{
	}
	//-------------------------------------------------------------------------------
	void RenderContext::WriteCommand(uint8_t command, void* data, uint32_t size, uint64_t sort_key)
	{
		SortCmd cmd;
		cmd.offset = (uint32_t)_data_stream.Tell();
		cmd.length = size + 1; // +1 for the command header
		cmd.sort_key = sort_key;
		cmd.buffer = &_cmd_buffer;

		_sort_cmds.push_back(cmd);

		_data_stream.Write(&command, 1);
		if (size)
		{
			_data_stream.Write(data, size);
		}
	}
	//-------------------------------------------------------------------------------
	void RenderContext::ClearContext()
	{
		_sort_cmds.clear();
		_data_stream.Seek(0);
	}
	//------------------------------------------------------------------------------
	void RenderContext::ClearState(uint64_t sort_key)
	{
		// Begin block by clearing the current device state
		WriteCommand(RC_CLEAR_STATE, nullptr, 0, sort_key);
	}
	//-------------------------------------------------------------------------------
	void RenderContext::SetTargets(uint64_t sort_key, const Targets& targets)
	{
		SetTargetsCmd cmd;
		for (uint32_t i = 0; i < MAX_MULTIPLE_RENDER_TARGETS; ++i)
		{
			cmd.render_target[i] = targets.render_targets[i].GetHandle();
		}
		cmd.depth_target = targets.depth_stencil_target.GetHandle();

		cmd.num_viewports = targets.num_viewports;
		cmd.viewport_offset = sizeof(SetTargetsCmd);

		cmd.num_scissor_rects = targets.num_scissor_rects;
		cmd.scissor_rect_offset = cmd.viewport_offset + sizeof(Viewport)* cmd.num_viewports;

		WriteCommand(RC_SET_TARGETS, &cmd, sizeof(SetTargetsCmd), sort_key);

		_data_stream.Write(targets.viewports, sizeof(Viewport)* cmd.num_viewports);
		_data_stream.Write(targets.rects, sizeof(ScissorRect)* cmd.num_scissor_rects);
	}
	//-------------------------------------------------------------------------------
	void RenderContext::ClearTargets(uint64_t sort_key, uint8_t flags, float color[4], float depth, uint8_t stencil)
	{
		ClearTargetsCmd	cmd;
		cmd.flags = flags;
		cmd.depth = depth;
		cmd.stencil = stencil;
		if (color != 0)
			memcpy(cmd.color, color, 4 * sizeof(float));

		WriteCommand(RC_CLEAR_TARGETS, &cmd, sizeof(ClearTargetsCmd), sort_key);
	}
	//-------------------------------------------------------------------------------
	void RenderContext::Draw(uint64_t sort_key, const RenderBlock& block, const ShaderContext& shader_context)
	{
		DrawCmd cmd;
		cmd.draw_call = block.draw_call;

		ShaderResources* shader_resources = shader_context.resources;
		Assert(shader_resources);

		uint32_t resource_offset = sizeof(DrawCmd);

		cmd.ia_resource_count = 0;
		if (IsValid(block.vertex_buffer.GetHandle())) ++cmd.ia_resource_count;
		if (IsValid(block.index_buffer.GetHandle())) ++cmd.ia_resource_count;
		if (IsValid(block.vertex_declaration.GetHandle())) ++cmd.ia_resource_count;

		cmd.ia_resource_offset = resource_offset;
		resource_offset += sizeof(RenderResource)*cmd.ia_resource_count; 

		Assert(shader_context.shader);
		cmd.shader = shader_context.shader->GetHandle();
		cmd.shader_resources_count = shader_resources->num_resources;
		cmd.shader_resources_offset = resource_offset;

		resource_offset += sizeof(RenderResource)* cmd.shader_resources_count;

		cmd.constant_buffer_count = shader_resources->num_constant_buffers;
		cmd.constant_buffer_offset = resource_offset;

		resource_offset += sizeof(RConstantBuffer)* shader_resources->num_constant_buffers;

		cmd.instance_data_size = shader_context.resources->instance_data_size;
		cmd.instance_data_offset = resource_offset;
		resource_offset += cmd.instance_data_size;
		cmd.constant_data_offset = resource_offset;


		WriteCommand(RC_DRAW, &cmd, sizeof(DrawCmd), sort_key);

		// Write IA resources to stream
		if (IsValid(block.vertex_buffer.GetHandle()))
			_data_stream.Write(&block.vertex_buffer, sizeof(RenderResource));
		if (IsValid(block.index_buffer.GetHandle()))
			_data_stream.Write(&block.index_buffer, sizeof(RenderResource));
		if (IsValid(block.vertex_declaration.GetHandle()))
			_data_stream.Write(&block.vertex_declaration, sizeof(RenderResource));

		// Write shader resources to stream
		_data_stream.Write(shader_resources->resources, cmd.shader_resources_count * sizeof(RenderResource));

		// Constant buffer info
		_data_stream.Write(shader_resources->constant_buffers, cmd.constant_buffer_count * sizeof(RConstantBuffer));

		// Instance data
		if (cmd.instance_data_size)
		{
			_data_stream.Write(shader_resources->instance_data, cmd.instance_data_size);
		}

		// Constant data
		uint32_t constant_data_size = 0; // Total constant data size
		for (uint32_t i = 0; i < cmd.constant_buffer_count; ++i)
		{
			const RConstantBuffer& constant_buffer = shader_resources->constant_buffers[i];

			// We don't do any updating of global cbuffers as they are updated separately
			if (constant_buffer.GetType() != RConstantBuffer::TYPE_GLOBAL)
			{
				constant_data_size += constant_buffer.GetSize();
			}
		}

		_data_stream.Write(shader_resources->constant_buffer_data, constant_data_size);

	}
	//-------------------------------------------------------------------------------

	void RenderContext::Dispatch(uint64_t sort_key, const ComputeCall& compute_call, const ShaderContext& shader_context)
	{
		DispatchCmd cmd;
		cmd.thread_group_count_x = compute_call.thread_group_count_x;
		cmd.thread_group_count_y = compute_call.thread_group_count_y;
		cmd.thread_group_count_z = compute_call.thread_group_count_z;

		ShaderResources* shader_resources = shader_context.resources;
		Assert(shader_resources);

		uint32_t offset = sizeof(DispatchCmd);

		Assert(shader_context.shader);
		cmd.shader = shader_context.shader->GetHandle();
		cmd.shader_resources_count = shader_resources->num_resources;
		cmd.shader_resources_offset = offset;

		offset += sizeof(RenderResource)* cmd.shader_resources_count;

		cmd.constant_buffer_count = shader_resources->num_constant_buffers;
		cmd.constant_buffer_offset = offset;

		offset += sizeof(RConstantBuffer)* shader_resources->num_constant_buffers;

		cmd.constant_data_offset = offset;

		WriteCommand(RC_DISPATCH, &cmd, sizeof(DispatchCmd), sort_key);

		// Write shader resources to stream
		_data_stream.Write(shader_resources->resources, cmd.shader_resources_count * sizeof(RenderResource));

		// Constant buffer info
		_data_stream.Write(shader_resources->constant_buffers, cmd.constant_buffer_count * sizeof(RConstantBuffer));

		// Constant data
		uint32_t constant_data_size = 0; // Total constant data size
		for (uint32_t i = 0; i < cmd.constant_buffer_count; ++i)
		{
			const RConstantBuffer& constant_buffer = shader_resources->constant_buffers[i];

			// We don't do any updating of global cbuffers as they are updated separately
			if (constant_buffer.GetType() != RConstantBuffer::TYPE_GLOBAL)
			{
				constant_data_size += constant_buffer.GetSize();
			}
		}

		_data_stream.Write(shader_resources->constant_buffer_data, constant_data_size);
	}

	//-------------------------------------------------------------------------------
	void RenderContext::UpdateBuffer(uint64_t sort_key, const RHardwareBuffer& buffer, void* data)
	{
		RenderResource::Type resource_type = buffer.GetType();

		if (resource_type != RenderResource::VERTEX_BUFFER &&
			resource_type != RenderResource::INDEX_BUFFER &&
			resource_type != RenderResource::CONSTANT_BUFFER &&
			resource_type != RenderResource::RAW_BUFFER)
		{
			Assert(false); // Not a supported type
			return;
		}

		UpdateBufferCmd cmd;
		cmd.resource = buffer;
		cmd.data_offset = sizeof(UpdateBufferCmd);

		uint32_t buffer_size = buffer.GetSize();

		WriteCommand(RC_UPDATE_BUFFER, &cmd, sizeof(UpdateBufferCmd), sort_key);
		_data_stream.Write(data, buffer_size);
	}

	//-------------------------------------------------------------------------------
	const RenderContext::SortCmdList& RenderContext::GetSortCmds() const
	{
		return _sort_cmds;
	}
	vector<uint8_t>* RenderContext::GetCmdBuffer()
	{
		return &_cmd_buffer;
	}
	//-------------------------------------------------------------------------------


} // namespace sb

