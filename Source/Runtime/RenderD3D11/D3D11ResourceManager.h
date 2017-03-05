// Copyright 2008-2014 Simon Ekström

#ifndef __D3D11ResourceManager_H__
#define __D3D11ResourceManager_H__

#include "D3D11Statistics.h"
#include <Engine/Rendering/RenderResourceAllocator.h>


namespace sb
{

	class RenderResourceAllocator;
	class RenderResource;
	class RRenderTarget;
	class RTexture;
	class RVertexBuffer;
	class RIndexBuffer;
	class RConstantBuffer;
	class RRawBuffer;
	class RShader;
	class RVertexDeclaration;
	class RenderResourceAllocator;

	template<typename T>
	class D3D11ShaderProgram;

	class D3D11RenderDevice;
	class D3D11DeviceContext;
	class D3D11HardwareBufferManager;
	class D3D11ShaderProgramManager;
	class D3D11TextureManager;
	class D3D11Shader;

	struct D3D11RenderTarget;
	struct D3D11Texture;
	struct D3D11VertexBuffer;
	struct D3D11IndexBuffer;
	struct D3D11ConstantBuffer;
	struct D3D11RawBuffer;
	struct D3D11VertexDeclaration;



	class D3D11_BLEND_DESC_COMPARE
	{
	public:
		bool operator()(const D3D11_BLEND_DESC& a, const D3D11_BLEND_DESC& b) const;
	};

	class D3D11_DEPTH_STENCIL_DESC_COMPARE
	{
	public:
		bool operator()(const D3D11_DEPTH_STENCIL_DESC& a, const D3D11_DEPTH_STENCIL_DESC& b) const;
	};

	class D3D11_RASTERIZER_DESC_COMPARE
	{
	public:
		bool operator()(const D3D11_RASTERIZER_DESC& a, const D3D11_RASTERIZER_DESC& b) const;
	};

	class D3D11_SAMPLER_DESC_COMPARE
	{
	public:
		bool operator()(const D3D11_SAMPLER_DESC& a, const D3D11_SAMPLER_DESC& b) const;
	};


	/// @brief Resource manager for D3D11 resources
	class D3D11ResourceManager
	{
	public:
		D3D11ResourceManager(D3D11RenderDevice* render_device);
		~D3D11ResourceManager();

		void FlushAllocator(RenderResourceAllocator* resource_allocator);

		/// @brief Allocates a render target
		/// @sa ReleaseResource
		void AllocateRenderTarget(uint32_t handle, const TextureDesc& desc, uint32_t bind_flags);
		void AllocateBackBuffer(uint32_t handle, const TextureDesc& desc, IDXGISwapChain* swap_chain);

		/// @brief Allocates a texture
		/// @sa ReleaseResource
		void AllocateTexture(uint32_t handle, const TextureDesc& desc, uint8_t** surface_data);

		/// @brief Allocates a vertex buffer
		/// @sa ReleaseResource
		void AllocateVertexBuffer(uint32_t handle, const VertexBufferDesc& desc, void* initial_data = 0);

		/// @brief Allocates a index buffer
		/// @sa ReleaseResource
		void AllocateIndexBuffer(uint32_t handle, const IndexBufferDesc& desc, void* initial_data = 0);

		/// @brief Allocates a constant buffer
		/// @sa ReleaseResource
		void AllocateConstantBuffer(uint32_t handle, uint32_t size, void* data);

		/// @brief Allocates a raw buffer
		/// @sa ReleaseResource
		void AllocateRawBuffer(uint32_t handle, const RawBufferDesc& desc, void* initial_data = 0);


		/// @brief Allocates a shader
		/// @sa ReleaseResource
		void AllocateShader(uint32_t handle, uint32_t platform_data_size, void* platform_data);

		/// @brief Allocates a vertex declaration
		/// @sa ReleaseResource
		void AllocateVertexDeclaration(const RVertexDeclaration& decl);


		/// @brief Releases a resource allocated with Allocate
		void ReleaseResource(const RenderResource& resource);

		/// @brief Updates a resource
		void UpdateResource(D3D11DeviceContext* context, RenderResource* resource, void* resource_data);


		D3D11RenderTarget* GetRenderTarget(uint32_t handle);
		D3D11VertexBuffer* GetVertexBuffer(uint32_t handle);
		D3D11IndexBuffer* GetIndexBuffer(uint32_t handle);
		D3D11ConstantBuffer* GetConstantBuffer(uint32_t handle);
		D3D11VertexDeclaration* GetVertexDeclaration(uint32_t handle);
		D3D11Shader* GetShader(uint32_t handle);

		/// Returns a blend state for the specified description, if no existing blend state exists
		///		the method will create a new one.
		/// @remark You're required to release the state when you're done with it.
		ID3D11BlendState* GetBlendState(const D3D11_BLEND_DESC& desc);

		/// Returns a depth stencil state for the specified description, if no existing depth stencil state exists
		///		the method will create a new one.
		/// @remark You're required to release the state when you're done with it.
		ID3D11DepthStencilState* GetDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& desc);

		/// Returns a rasterizer state for the specified description, if no existing rasterizer state exists
		///		the method will create a new one.
		/// @remark You're required to release the state when you're done with it.
		ID3D11RasterizerState* GetRasterizerState(const D3D11_RASTERIZER_DESC& desc);

		/// Returns a sampler state for the specified description, if no existing sampler state exists
		///		the method will create a new one.
		/// @remark You're required to release the state when you're done with it.
		ID3D11SamplerState* GetSamplerState(const D3D11_SAMPLER_DESC& desc);


		/// @brief Returns a input layout for the specified vertex declaration and vertex program
		/// This will try to find a input layout for the specified parameters, if no layout is found it will try to
		//		create a new one.
		ID3D11InputLayout* GetInputLayout(uint32_t vertex_decl, D3D11ShaderProgram<ID3D11VertexShader>* vertex_program);

		/// @brief Returns the shader resource view for the specified resource.
		/// @param srgb Indicates if the method should try to retrieve the view with SRGB enabled. (For textures)
		ID3D11ShaderResourceView* GetResourceView(uint32_t handle, bool srgb = false);

		/// @brief Returns the unordered access view for the specified resource.
		ID3D11UnorderedAccessView* GetUnorderedAccessView(uint32_t handle);

		D3D11ShaderProgramManager* GetShaderProgramManager();
		D3D11HardwareBufferManager* GetHardwareBufferManager();

		D3D11MemoryStatistics* GetMemoryStatistics();

	private:
		void ReleaseRenderTarget(uint32_t handle);
		void ReleaseTexture(uint32_t handle);
		void ReleaseHardwareBuffer(uint32_t type, uint32_t handle);
		void ReleaseVertexDeclaration(uint32_t handle);
		void ReleaseShader(uint32_t handle);

		/// Clears all render states in manager
		void ClearStates();

		/// Builds a handle from a resource type and a index for the handle lookup table 
		INLINE uint32_t BuildHandle(uint16_t type, uint32_t index)
		{
			return uint32_t(type << 16) | index;
		}
		/// Returns the index from the specified handle, also asserts if the index doesn't belong to the specified resource type
		INLINE uint32_t GetIndex(uint32_t handle, uint16_t type)
		{
			Verify(uint16_t(handle >> 16) == type);
			return (handle & 0xffff);
		}

	private:
		typedef map<D3D11_BLEND_DESC, ComPtr<ID3D11BlendState>, D3D11_BLEND_DESC_COMPARE> BlendStateMap;
		typedef map<D3D11_DEPTH_STENCIL_DESC, ComPtr<ID3D11DepthStencilState>, D3D11_DEPTH_STENCIL_DESC_COMPARE> DepthStencilStateMap;
		typedef map<D3D11_RASTERIZER_DESC, ComPtr<ID3D11RasterizerState>, D3D11_RASTERIZER_DESC_COMPARE> RasterizerStateMap;
		typedef map<D3D11_SAMPLER_DESC, ComPtr<ID3D11SamplerState>, D3D11_SAMPLER_DESC_COMPARE> SamplerStateMap;

		D3D11RenderDevice*	_device;
		D3D11HardwareBufferManager* _hw_buffer_manager;
		D3D11ShaderProgramManager* _shader_program_manager;
		D3D11TextureManager* _texture_manager;
		D3D11MemoryStatistics _memory_statistics;

		uint32_t _num_allocations;

		vector<uint32_t> _handle_lut; /// Table for handle -> internal handle (16 bit for resource type, 16 bit for index)
		MemoryPool<D3D11RenderTarget, 64> _render_target_pool;
		MemoryPool<D3D11Texture, 64> _texture_pool;
		MemoryPool<D3D11VertexBuffer, 64> _vertex_buffer_pool;
		MemoryPool<D3D11IndexBuffer, 64> _index_buffer_pool;
		MemoryPool<D3D11ConstantBuffer, 64> _constant_buffer_pool;
		MemoryPool<D3D11RawBuffer, 64> _raw_buffer_pool;
		MemoryPool<D3D11Shader, 64> _shader_pool;
		MemoryPool<D3D11VertexDeclaration, 64> _vertex_declaration_pool;

		BlendStateMap _blend_states;
		DepthStencilStateMap _depth_stencil_states;
		RasterizerStateMap _rasterizer_states;
		SamplerStateMap _sampler_states;

		CriticalSection _input_layout_lock; ///! Lock used to avoid race conditions when looking up input layouts

	};

} // namespace sb




#endif // __D3D11ResourceManager_H__
