// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_RENDERRESOURCEALLOCATOR_H__
#define __RENDERING_RENDERRESOURCEALLOCATOR_H__

#include "HandleGenerator.h"

#include "RTexture.h"
#include "RRenderTarget.h"
#include "RVertexBuffer.h"
#include "RIndexBuffer.h"
#include "RConstantBuffer.h"
#include "RRawBuffer.h"
#include "RVertexDeclaration.h"
#include "RShader.h"

#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Memory/LinearAllocator.h>

namespace sb
{

	class RenderDevice;
	class RenderResourceAllocator
	{
	public:
		enum
		{
			/// @sa AllocateTextureMsg
			ALLOCATE_TEXTURE,
			/// @sa ResourceMessage
			RELEASE_TEXTURE,

			/// @sa AllocateRenderTargetMsg
			ALLOCATE_RENDER_TARGET,
			/// @sa ResourceMessage
			RELEASE_RENDER_TARGET,

			/// @sa AllocateVertexBufferMsg
			ALLOCATE_VERTEX_BUFFER,
			/// @sa ResourceMessage
			RELEASE_VERTEX_BUFFER,

			/// @sa AllocateIndexBufferMsg
			ALLOCATE_INDEX_BUFFER,
			/// @sa ResourceMessage
			RELEASE_INDEX_BUFFER,

			/// @sa AllocateConstantBufferMsg
			ALLOCATE_CONSTANT_BUFFER,
			/// @sa ResourceMessage
			RELEASE_CONSTANT_BUFFER,

			/// @sa AllocateShaderMsg
			ALLOCATE_SHADER,
			/// @sa ResourceMessage
			RELEASE_SHADER,

			/// @sa AllocateVertexDeclarationMsg
			ALLOCATE_VERTEX_DECLARATION,
			/// @sa ResourceMessage
			RELEASE_VERTEX_DECLARATION,

			/// @sa AllocateRawBufferMsg
			ALLOCATE_RAW_BUFFER,
			/// @sa ResourceMessage
			RELEASE_RAW_BUFFER
		};

		struct ResourceCommand
		{
			uint32_t resource_handle;
		};

		struct AllocateTextureCmd : public ResourceCommand
		{
			TextureDesc desc;

			uint32_t num_surfaces;
			uint8_t** surface_data;
		};

		struct AllocateRenderTargetCmd : public ResourceCommand
		{
			TextureDesc desc;
			uint32_t bind_flags;
		};

		struct AllocateVertexBufferCmd : public ResourceCommand
		{
			VertexBufferDesc desc;
			void* data; // Initial data
		};

		struct AllocateIndexBufferCmd : public ResourceCommand
		{
			IndexBufferDesc desc;
			void* data; // Initial data
		};

		struct AllocateConstantBufferCmd : public ResourceCommand
		{
			uint32_t size; // Size of constant buffer
			void* data; // Initial data
		};

		struct AllocateShaderCmd : public ResourceCommand
		{
			uint32_t platform_data_size;
			void* platform_data;
		};

		struct AllocateVertexDeclarationCmd : public ResourceCommand
		{
			RVertexDeclaration vertex_declaration;
		};

		struct AllocateRawBufferCmd : public ResourceCommand
		{
			RawBufferDesc desc;
			void* data; // Initial data
		};



	public:
		RenderResourceAllocator(RenderDevice* device);
		~RenderResourceAllocator();

		/// @brief Allocates a new texture from the specified description.
		/// @param surfaces Optional texture surface data.
		void AllocateTexture(RTexture& texture, const vector<image::Surface>* surface_data = nullptr);

		/// @brief Allocates a new render target from the specified description.
		void AllocateRenderTarget(RRenderTarget& render_target);

		/// @brief Allocates a new vertex buffer from the specified description.
		/// @param initial_data Initial data to fill the buffer with.
		void AllocateVertexBuffer(RVertexBuffer& buffer, void* initial_data = nullptr);

		/// @brief Allocates a new index buffer from the specified description.
		/// @param initial_data Initial data to fill the buffer with.
		void AllocateIndexBuffer(RIndexBuffer& buffer, void* initial_data = nullptr);

		/// @brief Allocates a new constant buffer of the specified size.
		/// @param initial_data Initial data to fill the buffer with.
		void AllocateConstantBuffer(RConstantBuffer& buffer, void* initial_data = nullptr);

		/// @brief Allocates a shader object.
		void AllocateShader(RShader& shader_data);

		/// @brief Allocates a vertex declaration object from the specified vertex declaration.
		void AllocateVertexDeclaration(RVertexDeclaration& vertex_declaration);

		/// @brief Allocates a new raw buffer of the specified size.
		/// @param initial_data Initial data to fill the buffer with.
		void AllocateRawBuffer(RRawBuffer& buffer, void* initial_data = nullptr);

		/// @brief Releases the specified render resource
		/// The specified resource handle should be considered invalid after this call.
		void ReleaseResource(RenderResource& resource);

		/// @brief Locks the allocator, prevents any new commands.
		/// @sa Unlock
		void Lock();

		/// @brief Unlocks the allocator.
		/// @sa Lock
		void Unlock();

		/// @brief Clears the allocator, removing any queued commands.
		/// @remark This operation will not issue any locks internally, so make sure to 
		///				use Lock and Unlock before and after to keep it thread-safe.
		void Clear();

		/// @brief Returns the allocators current command buffer.
		/// To make sure no changes are made while you're reading the command buffer, consider using Lock and Unlock.
		const vector<uint8_t>& CommandBuffer() const;

		HandleGenerator& GetHandleGenerator();

	private:
		RenderDevice* _device;
		HandleGenerator _handle_generator;

		CriticalSection _lock;

		vector<uint8_t> _cmd_buffer;
		DynamicMemoryStream	_data_stream; ///< Helper class just to make writing a bit easier

		LinearAllocator _resource_data_allocator;
	};


} // namespace sb


#endif // __RENDERING_RENDERRESOURCEALLOCATOR_H__
