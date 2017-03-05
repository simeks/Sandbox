// Copyright 2008-2014 Simon Ekström

#ifndef __D3D11TEXTUREMANAGER_H__
#define __D3D11TEXTUREMANAGER_H__

#include <Engine/Rendering/Texture.h>


namespace sb
{

	struct D3D11RenderTarget;
	struct D3D11Texture;
	struct D3D11MemoryStatistics;

	class D3D11RenderDevice;
	class D3D11TextureManager
	{
	public:
		D3D11TextureManager(D3D11RenderDevice* device, D3D11MemoryStatistics* statistics);
		~D3D11TextureManager();

		void CreateTexture(D3D11Texture* d3d_texture, const TextureDesc& desc, uint8_t** surface_data);
		void DestroyTexture(D3D11Texture* d3d_texture);

		void CreateRenderTarget(D3D11RenderTarget* d3d_target, const TextureDesc& desc, uint32_t bind_flags);
		void DestroyRenderTarget(D3D11RenderTarget* d3d_target);

		void CreateBackBuffer(D3D11RenderTarget* d3d_target, IDXGISwapChain* swap_chain, const TextureDesc& desc);
		void DestroyBackBuffer(D3D11RenderTarget* d3d_target);

		/// Converts a PixelFormat to DXGI_FORMAT
		/// @param srgb True for SRGB support
		DXGI_FORMAT ToDXGIFormat(image::PixelFormat fmt, bool srgb);

		/// Converts a PixelFormat to a typeless DXGI_FORMAT
		DXGI_FORMAT ToDXGIFormatTypeless(image::PixelFormat fmt);

	private:
		D3D11RenderDevice* _device;
		D3D11MemoryStatistics* _statistics;

	};

} // namespace sb

#endif // __D3D11TEXTUREMANAGER_H__
