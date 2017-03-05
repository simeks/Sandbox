// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "D3D11InstanceMerger.h"
#include "D3D11DeviceContext.h"
#include "D3D11RenderDevice.h"
#include "D3D11ResourceManager.h"
#include "D3D11Shader.h"

#include <Foundation/Profiler/Profiler.h>


namespace sb
{

	//-------------------------------------------------------------------------------
	D3D11InstanceMerger::D3D11InstanceMerger(D3D11RenderDevice* device)
		: _device(device)
	{
		_instance_data_buffer_desc.elem_size = 0;
		_instance_data_buffer_desc.elem_type = RawBufferDesc::ET_FLOAT4;
		_instance_data_buffer_desc.size = INITIAL_INSTANCE_BUFFER_SIZE;
		_instance_data_buffer_desc.usage = hardware_buffer::DYNAMIC;
		_instance_data_buffer = RRawBuffer(_instance_data_buffer_desc);

		_instance_data.reserve(INITIAL_INSTANCE_BUFFER_SIZE);

		RenderResourceAllocator* resource_allocator = _device->GetResourceAllocator();
		resource_allocator->AllocateRawBuffer(_instance_data_buffer);
		_device->FlushAllocator();
	}
	D3D11InstanceMerger::~D3D11InstanceMerger()
	{
		RenderResourceAllocator* resource_allocator = _device->GetResourceAllocator();
		resource_allocator->ReleaseResource(_instance_data_buffer);
		_device->FlushAllocator();
	}
	//-------------------------------------------------------------------------------
	void D3D11InstanceMerger::Add(RenderContext::DrawCmd* cmd, const uint64_t& sort_key)
	{
		// Shift to the right as the bits to the right from the instance bit aren't accounted for when batching.
		uint64_t batch_key = sort_key >> render_sorting::INSTANCE_BIT;
		// Move them back so that we get all 0s to the right of the instance bit.
		batch_key = batch_key << render_sorting::INSTANCE_BIT;

		Batch& batch = GetBatch(batch_key);
		batch.cmds.push_back(cmd);
	}
	//-------------------------------------------------------------------------------
	void D3D11InstanceMerger::Dispatch(D3D11DeviceContext* context)
	{
		for (auto& batch : _batches)
		{
			if (!batch.cmds.size())
				continue;

			RenderContext::DrawCmd* draw_cmd = batch.cmds.front();
			if (batch.sort_key & uint64_t(1 << render_sorting::INSTANCE_BIT))
			{
				PROFILER_SCOPE("UpdateInstanceData");

				if (draw_cmd->draw_call.instance_count == 0)
					draw_cmd->draw_call.instance_count = (uint32_t)batch.cmds.size();

				// Bind shader variables

				uint64_t shader_pass_id = batch.sort_key << (64 - render_sorting::SHADER_PASS_BIT - render_sorting::SHADER_PASS_NUM_BITS);
				shader_pass_id = shader_pass_id >> (64 - render_sorting::SHADER_PASS_NUM_BITS);

				D3D11Shader* shader = _device->GetD3D11ResourceManager()->GetShader(draw_cmd->shader);
				Assert(shader);
				D3D11ShaderPass* shader_pass = shader->GetShaderPass((uint32_t)shader_pass_id);
				Assert(shader_pass);

				// Fill buffer, TODO: Could we increase performance by sharing the buffer for all batches rather than updating per batch?
				_instance_data.resize(shader_pass->GetData()->instance_data.size * draw_cmd->draw_call.instance_count);
				void* data_dest = _instance_data.data();

				for (auto& draw_cmd : batch.cmds)
				{
					void* data_src = memory::PointerAdd(draw_cmd, (draw_cmd->instance_data_offset)); 

					for (auto& var : shader_pass->GetData()->instance_data.variables)
					{
						// Copy variable from source to destination (the instance data buffer)
						memcpy(memory::PointerAdd(data_dest, var.dest_offset), memory::PointerAdd(data_src, var.src_offset), var.size);
					}

					data_dest = memory::PointerAdd(data_dest, draw_cmd->instance_data_size);
				}



				if (_instance_data_buffer.GetSize() < _instance_data.size())
				{
					RenderResourceAllocator* resource_allocator = _device->GetResourceAllocator();
					resource_allocator->ReleaseResource(_instance_data_buffer);

					_instance_data_buffer = RRawBuffer(_instance_data_buffer_desc);
					resource_allocator->AllocateRawBuffer(_instance_data_buffer, _instance_data.data());

					_device->FlushAllocator();
				}
				else
				{
					_device->GetD3D11ResourceManager()->UpdateResource(context, &_instance_data_buffer, _instance_data.data());
				}

				uint32_t instance_buffer_index = shader_pass->GetData()->instance_data.instance_data_slot;

				// Bind instance data buffer to shader
				if (IsValid(instance_buffer_index))
				{
					RenderResource* instance_data_slot = (RenderResource*)memory::PointerAdd(draw_cmd, draw_cmd->shader_resources_offset) + instance_buffer_index;
					*instance_data_slot = _instance_data_buffer;
				}

			}
			context->Draw(draw_cmd, batch.sort_key);
		}

		_batches.clear();
	}
	//-------------------------------------------------------------------------------
	uint32_t D3D11InstanceMerger::Size() const
	{
		return (uint32_t)_batches.size();
	}

	//-------------------------------------------------------------------------------
	D3D11InstanceMerger::Batch& D3D11InstanceMerger::GetBatch(const uint64_t& sort_key)
	{
		// Check if batch is supposed to use instancing
		//if (sort_key & uint64_t(1 << render_sorting::INSTANCE_BIT))
		//{
		//	vector<Batch>::Iterator it, end;
		//	it = _batches.Begin(); end = _batches.End();
		//	for (; it != end; ++it)
		//	{
		//		if (it->sort_key == sort_key)
		//		{
		//			// We don't want to merge with batches that have an initial instance count set.
		//			if (it->cmds.Size() && it->cmds.Front()->draw_call.instance_count != 0)
		//				continue;

		//			// Batch found
		//			return *it;
		//		}
		//	}

		//}

		// TODO: How to find matching batches?

		// Create new batch
		_batches.push_back(Batch());
		_batches.back().sort_key = sort_key;
		return _batches.back();
	}

	//-------------------------------------------------------------------------------

} // namespace sb
