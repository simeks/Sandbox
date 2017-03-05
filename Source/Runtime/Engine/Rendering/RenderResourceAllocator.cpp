// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "RenderResourceAllocator.h"
#include "RenderDevice.h"

namespace sb
{

	RenderResourceAllocator::RenderResourceAllocator(RenderDevice* device)
		: _device(device),
		_data_stream(&_cmd_buffer),
		_resource_data_allocator(2048 * 2048)
	{
	}
	RenderResourceAllocator::~RenderResourceAllocator()
	{
	}

	void RenderResourceAllocator::AllocateTexture(RTexture& texture, const vector<image::Surface>* surface_data)
	{
		ScopedLock<CriticalSection> scoped_lock(_lock);

		uint32_t resource_handle = _handle_generator.New();
		texture.SetHandle(resource_handle);

		AllocateTextureCmd cmd;
		cmd.resource_handle = resource_handle;
		cmd.desc = texture.GetDesc();
		cmd.num_surfaces = 0;

		if (surface_data && surface_data->size() > 0)
		{
			cmd.num_surfaces = (uint32_t)surface_data->size();
			cmd.surface_data = (uint8_t**)_resource_data_allocator.Allocate(sizeof(uint8_t*)* cmd.num_surfaces);

			for (uint32_t i = 0; i < cmd.num_surfaces; ++i)
			{
				uint32_t surface_size = (*surface_data)[i].size;
				cmd.surface_data[i] = (uint8_t*)_resource_data_allocator.Allocate(surface_size);
				memcpy(cmd.surface_data[i], (*surface_data)[i].data, (*surface_data)[i].size);
			}
		}
		uint8_t header = ALLOCATE_TEXTURE;
		_data_stream.Write(&header, 1);
		_data_stream.Write(&cmd, sizeof(AllocateTextureCmd));

	}
	void RenderResourceAllocator::AllocateRenderTarget(RRenderTarget& render_target)
	{
		ScopedLock<CriticalSection> scoped_lock(_lock);

		uint32_t resource_handle = _handle_generator.New();
		render_target.SetHandle(resource_handle);

		AllocateRenderTargetCmd cmd;
		cmd.resource_handle = resource_handle;
		cmd.desc = render_target.GetDesc();
		cmd.bind_flags = render_target.GetBindFlags();

		uint8_t header = ALLOCATE_RENDER_TARGET;
		_data_stream.Write(&header, 1);
		_data_stream.Write(&cmd, sizeof(AllocateRenderTargetCmd));

	}
	void RenderResourceAllocator::AllocateVertexBuffer(RVertexBuffer& buffer, void* initial_data)
	{
		ScopedLock<CriticalSection> scoped_lock(_lock);

		uint32_t resource_handle = _handle_generator.New();
		buffer.SetHandle(resource_handle);

		AllocateVertexBufferCmd cmd;
		cmd.resource_handle = resource_handle;
		cmd.desc = buffer.GetDesc();
		cmd.data = nullptr;

		uint32_t buffer_size = cmd.desc.vertex_count * cmd.desc.vertex_size;
		if (buffer_size && initial_data)
		{
			cmd.data = _resource_data_allocator.Allocate(buffer_size);
			memcpy(cmd.data, initial_data, buffer_size);
		}

		uint8_t header = ALLOCATE_VERTEX_BUFFER;
		_data_stream.Write(&header, 1);
		_data_stream.Write(&cmd, sizeof(AllocateVertexBufferCmd));
	}
	void RenderResourceAllocator::AllocateIndexBuffer(RIndexBuffer& buffer, void* initial_data)
	{
		ScopedLock<CriticalSection> scoped_lock(_lock);

		uint32_t resource_handle = _handle_generator.New();
		buffer.SetHandle(resource_handle);

		AllocateIndexBufferCmd cmd;
		cmd.resource_handle = resource_handle;
		cmd.desc = buffer.GetDesc();
		cmd.data = nullptr;

		uint32_t buffer_size = cmd.desc.index_count * (cmd.desc.index_format == index_buffer::INDEX_32 ? 4 : 2);
		if (buffer_size && initial_data)
		{
			cmd.data = _resource_data_allocator.Allocate(buffer_size);
			memcpy(cmd.data, initial_data, buffer_size);
		}

		uint8_t header = ALLOCATE_INDEX_BUFFER;
		_data_stream.Write(&header, 1);
		_data_stream.Write(&cmd, sizeof(AllocateIndexBufferCmd));
	}
	void RenderResourceAllocator::AllocateConstantBuffer(RConstantBuffer& buffer, void* initial_data)
	{
		ScopedLock<CriticalSection> scoped_lock(_lock);

		uint32_t resource_handle = _handle_generator.New();
		buffer.SetHandle(resource_handle);

		AllocateConstantBufferCmd cmd;
		cmd.resource_handle = resource_handle;
		cmd.size = buffer.GetSize();
		cmd.data = nullptr;

		if (initial_data && cmd.size)
		{
			// Copy over the provided initial data to a separate temporary buffer.
			cmd.data = _resource_data_allocator.Allocate(cmd.size);
			memcpy(cmd.data, initial_data, cmd.size);
		}

		uint8_t header = ALLOCATE_CONSTANT_BUFFER;
		_data_stream.Write(&header, 1);
		_data_stream.Write(&cmd, sizeof(AllocateConstantBufferCmd));

	}
	void RenderResourceAllocator::AllocateShader(RShader& shader)
	{
		ScopedLock<CriticalSection> scoped_lock(_lock);

		uint32_t resource_handle = _handle_generator.New();
		shader.SetHandle(resource_handle);

		AllocateShaderCmd cmd;
		cmd.resource_handle = resource_handle;
		cmd.platform_data_size = shader.GetPlatformDataSize();

		// Create a temporary copy of the platform data
		cmd.platform_data = _resource_data_allocator.Allocate(cmd.platform_data_size);
		memcpy(cmd.platform_data, shader.GetPlatformData(), cmd.platform_data_size);

		uint8_t header = ALLOCATE_SHADER;
		_data_stream.Write(&header, 1);
		_data_stream.Write(&cmd, sizeof(AllocateShaderCmd));
	}
	void RenderResourceAllocator::AllocateVertexDeclaration(RVertexDeclaration& vertex_declaration)
	{
		ScopedLock<CriticalSection> scoped_lock(_lock);

		uint32_t resource_handle = _handle_generator.New();
		vertex_declaration.SetHandle(resource_handle);

		AllocateVertexDeclarationCmd cmd;
		cmd.resource_handle = resource_handle;
		cmd.vertex_declaration = vertex_declaration;

		uint8_t header = ALLOCATE_VERTEX_DECLARATION;
		_data_stream.Write(&header, 1);
		_data_stream.Write(&cmd, sizeof(AllocateVertexDeclarationCmd));

	}
	void RenderResourceAllocator::AllocateRawBuffer(RRawBuffer& buffer, void* initial_data)
	{
		ScopedLock<CriticalSection> scoped_lock(_lock);

		uint32_t resource_handle = _handle_generator.New();
		buffer.SetHandle(resource_handle);

		AllocateRawBufferCmd cmd;
		cmd.resource_handle = resource_handle;
		cmd.desc = buffer.GetDesc();
		cmd.data = nullptr;

		if (initial_data && cmd.desc.size)
		{
			// Copy over the provided initial data to a separate temporary buffer.
			cmd.data = _resource_data_allocator.Allocate(cmd.desc.size);
			memcpy(cmd.data, initial_data, cmd.desc.size);
		}

		uint8_t header = ALLOCATE_RAW_BUFFER;
		_data_stream.Write(&header, 1);
		_data_stream.Write(&cmd, sizeof(AllocateRawBufferCmd));

	}

	void RenderResourceAllocator::ReleaseResource(RenderResource& resource)
	{
		if (IsInvalid(resource.GetHandle()))
			return;

		{
			ScopedLock<CriticalSection> scoped_lock(_lock);

			uint8_t header;
			switch (resource.GetType())
			{
			case RenderResource::RENDER_TARGET:
				header = RELEASE_RENDER_TARGET;
				break;
			case RenderResource::TEXTURE:
				header = RELEASE_TEXTURE;
				break;
			case RenderResource::VERTEX_BUFFER:
				header = RELEASE_VERTEX_BUFFER;
				break;
			case RenderResource::INDEX_BUFFER:
				header = RELEASE_INDEX_BUFFER;
				break;
			case RenderResource::CONSTANT_BUFFER:
				header = RELEASE_CONSTANT_BUFFER;
				break;
			case RenderResource::SHADER:
				header = RELEASE_SHADER;
				break;
			case RenderResource::VERTEX_DECLARATION:
				header = RELEASE_VERTEX_DECLARATION;
				break;
			case RenderResource::RAW_BUFFER:
				header = RELEASE_RAW_BUFFER;
				break;
			default:
				return;
			};

			_data_stream.Write(&header, 1);

			ResourceCommand cmd;
			cmd.resource_handle = resource.GetHandle();

			_data_stream.Write(&cmd, sizeof(ResourceCommand));
		}

		_handle_generator.Release(resource.GetHandle());
		resource.SetHandle(Invalid<uint32_t>());
	}

	void RenderResourceAllocator::Lock()
	{
		_lock.Lock();
	}
	void RenderResourceAllocator::Unlock()
	{
		_lock.Unlock();
	}
	void RenderResourceAllocator::Clear()
	{
		_cmd_buffer.clear();
		_data_stream.Seek(0);

		_resource_data_allocator.Reset();
	}
	const vector<uint8_t>& RenderResourceAllocator::CommandBuffer() const
	{
		return _cmd_buffer;
	}
	HandleGenerator& RenderResourceAllocator::GetHandleGenerator()
	{
		return _handle_generator;
	}


} // namespace sb
