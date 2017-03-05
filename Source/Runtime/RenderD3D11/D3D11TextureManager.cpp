// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "D3D11TextureManager.h"
#include "D3D11RenderDevice.h"
#include "D3D11Texture.h"
#include "D3D11RenderTarget.h"
#include "D3D11Statistics.h"

#include <Engine/Rendering/RTexture.h>
#include <Engine/Rendering/RRenderTarget.h>


namespace sb
{

	namespace
	{
		uint32_t CalcSysMemPitch(image::PixelFormat fmt, uint32_t width)
		{
			switch (fmt)
			{
			case image::PF_BC1:
			case image::PF_BC4:
				return Max<uint32_t>(1, width / 4) * 8;

			case image::PF_BC2:
			case image::PF_BC3:
			case image::PF_BC5:
				return Max<uint32_t>(1, width / 4) * 16;

			default:
				return (width * image::BitsPerPixel(fmt) + 7) / 8; // round up to nearest byte
			}

		}
	}


	//-------------------------------------------------------------------------------
	D3D11TextureManager::D3D11TextureManager(D3D11RenderDevice* device, D3D11MemoryStatistics* statistics)
		: _device(device),
		_statistics(statistics)
	{
	}
	D3D11TextureManager::~D3D11TextureManager()
	{

	}

	//-------------------------------------------------------------------------------

	void D3D11TextureManager::CreateTexture(D3D11Texture* d3d_texture, const TextureDesc& desc, uint8_t** surface_data)
	{
		ComPtr<ID3D11Device> device = _device->GetD3DDevice();
		switch (desc.type)
		{
		case TextureDesc::TYPE_2D:
		{
			D3D11_TEXTURE2D_DESC d3d_desc;

			// Should we try to create a srgb resource view for this texture
			bool srgb = (desc.pixel_format == image::PF_R8G8B8A8)
				|| (desc.pixel_format == image::PF_BC1)
				|| (desc.pixel_format == image::PF_BC2)
				|| (desc.pixel_format == image::PF_BC3);


			d3d_desc.Width = desc.width;
			d3d_desc.Height = desc.height;
			d3d_desc.MipLevels = desc.mip_count;
			d3d_desc.ArraySize = 1;
			d3d_desc.Format = ToDXGIFormatTypeless(desc.pixel_format);
			Assert(d3d_desc.Format != DXGI_FORMAT_UNKNOWN);

			d3d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			d3d_desc.SampleDesc.Count = 1;
			d3d_desc.SampleDesc.Quality = 0;
			d3d_desc.MiscFlags = 0;

			if (desc.usage == TextureDesc::DYNAMIC)
			{
				d3d_desc.Usage = D3D11_USAGE_DYNAMIC;
				d3d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			}
			else // Static
			{
				d3d_desc.Usage = D3D11_USAGE_IMMUTABLE;
				d3d_desc.CPUAccessFlags = 0;
			}

			D3D11_SUBRESOURCE_DATA* resource_data = new D3D11_SUBRESOURCE_DATA[d3d_desc.ArraySize * d3d_desc.MipLevels]; // TODO: Temp alloc

			d3d_texture->size = 0; // Total texture size in bytes
			uint32_t index = 0;
			for (uint32_t i = 0; i < d3d_desc.ArraySize; ++i)
			{
				uint32_t w = desc.width;
				uint32_t h = desc.height;
				for (uint32_t m = 0; m < d3d_desc.MipLevels; ++m)
				{
					resource_data[index].pSysMem = surface_data[index];
					resource_data[index].SysMemPitch = CalcSysMemPitch(desc.pixel_format, w);
					d3d_texture->size += resource_data[index].SysMemPitch * h;

					if (w > 1)
						w = w >> 1;
					if (h > 1)
						h = h >> 1;

					++index;
				}
			}

			D3D11Texture2D* d3d_tex_impl = new D3D11Texture2D();
			Assert(d3d_tex_impl);
			D3D_VERIFY(device->CreateTexture2D(&d3d_desc, resource_data, &d3d_tex_impl->texture));

			delete[] resource_data; // TODO: Temp alloc

			// Create the standard resource view
			D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
			srv_desc.Format = ToDXGIFormat(desc.pixel_format, false);
			srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srv_desc.Texture2D.MipLevels = 1;
			srv_desc.Texture2D.MostDetailedMip = 0;

			D3D_VERIFY(device->CreateShaderResourceView(d3d_tex_impl->texture.Get(), &srv_desc, &d3d_texture->srv));

			// Try create a SRGB view for the texture if possible
			if (srgb)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC srv_srgb_desc;
				srv_srgb_desc.Format = ToDXGIFormat(desc.pixel_format, true);
				srv_srgb_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srv_srgb_desc.Texture2D.MipLevels = 1;
				srv_srgb_desc.Texture2D.MostDetailedMip = 0;

				D3D_VERIFY(device->CreateShaderResourceView(d3d_tex_impl->texture.Get(), &srv_srgb_desc, &d3d_texture->srv_srgb));
			}
			else
			{
				d3d_texture->srv_srgb = NULL;
			}

			d3d_texture->data = d3d_tex_impl;

			break;
		}
		case TextureDesc::TYPE_3D:
			Assert(false); // not implemented
			break;
		case TextureDesc::TYPE_CUBE:
		{
			Assert(desc.array_size == 6);

			D3D11_TEXTURE2D_DESC d3d_desc;

			// Should we try to create a srgb resource view for this texture
			bool srgb = (desc.pixel_format == image::PF_R8G8B8A8)
				|| (desc.pixel_format == image::PF_BC1)
				|| (desc.pixel_format == image::PF_BC2)
				|| (desc.pixel_format == image::PF_BC3);


			d3d_desc.Width = desc.width;
			d3d_desc.Height = desc.height;
			d3d_desc.MipLevels = desc.mip_count;
			d3d_desc.ArraySize = desc.array_size;
			d3d_desc.Format = ToDXGIFormatTypeless(desc.pixel_format);
			Assert(d3d_desc.Format != DXGI_FORMAT_UNKNOWN);

			d3d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			d3d_desc.SampleDesc.Count = 1;
			d3d_desc.SampleDesc.Quality = 0;
			d3d_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

			if (desc.usage == TextureDesc::DYNAMIC)
			{
				d3d_desc.Usage = D3D11_USAGE_DYNAMIC;
				d3d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			}
			else // Static
			{
				d3d_desc.Usage = D3D11_USAGE_IMMUTABLE;
				d3d_desc.CPUAccessFlags = 0;
			}

			D3D11_SUBRESOURCE_DATA* resource_data = new D3D11_SUBRESOURCE_DATA[d3d_desc.ArraySize * d3d_desc.MipLevels]; // TODO: Temp alloc

			d3d_texture->size = 0; // Total texture size in bytes
			uint32_t index = 0;
			for (uint32_t i = 0; i < d3d_desc.ArraySize; ++i)
			{
				uint32_t w = desc.width;
				uint32_t h = desc.height;
				for (uint32_t m = 0; m < d3d_desc.MipLevels; ++m)
				{
					resource_data[index].pSysMem = surface_data[index];
					resource_data[index].SysMemPitch = CalcSysMemPitch(desc.pixel_format, w);
					d3d_texture->size += resource_data[index].SysMemPitch * h;

					if (w > 1)
						w = w >> 1;
					if (h > 1)
						h = h >> 1;

					++index;
				}
			}

			D3D11Texture2D* d3d_tex_impl = new D3D11Texture2D();
			D3D_VERIFY(device->CreateTexture2D(&d3d_desc, resource_data, &d3d_tex_impl->texture));

			delete[] resource_data; // TODO: Temp alloc

			// Create the standard resource view
			D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
			srv_desc.Format = ToDXGIFormat(desc.pixel_format, false);
			srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			srv_desc.TextureCube.MipLevels = 1;
			srv_desc.TextureCube.MostDetailedMip = 0;

			D3D_VERIFY(device->CreateShaderResourceView(d3d_tex_impl->texture.Get(), &srv_desc, &d3d_texture->srv));

			// Try create a SRGB view for the texture if possible
			if (srgb)
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC srv_srgb_desc;
				srv_srgb_desc.Format = ToDXGIFormat(desc.pixel_format, true);
				srv_srgb_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
				srv_srgb_desc.TextureCube.MipLevels = 1;
				srv_srgb_desc.TextureCube.MostDetailedMip = 0;

				D3D_VERIFY(device->CreateShaderResourceView(d3d_tex_impl->texture.Get(), &srv_srgb_desc, &d3d_texture->srv_srgb));
			}
			else
			{
				d3d_texture->srv_srgb = NULL;
			}

			d3d_texture->data = d3d_tex_impl;

			break;
		}
		default:
			Assert(false);

		};

		// Update statistics
		_statistics->texture_memory += d3d_texture->size;

	}
	void D3D11TextureManager::DestroyTexture(D3D11Texture* d3d_texture)
	{
		switch (d3d_texture->data->type)
		{
		case D3D11Resource::TEXTURE2D:
		{
			delete d3d_texture->data;

			break;
		}
		case D3D11Resource::TEXTURE3D:
			Assert(false); // not implemented
			break;
			//case D3D11Resource::TEXTURECUBE:
			//	Assert(false); // not implemented
			//	break;
		default:
			Assert(false);
		};

		// Update statistics
		_statistics->texture_memory -= d3d_texture->size;
		d3d_texture->size = 0;
	}

	//-------------------------------------------------------------------------------

	void D3D11TextureManager::CreateRenderTarget(D3D11RenderTarget* d3d_target, const TextureDesc& desc, uint32_t bind_flags)
	{
		ComPtr<ID3D11Device> device = _device->GetD3DDevice();

		// First create the target texture
		switch (desc.type)
		{
		case TextureDesc::TYPE_2D:
		{
			// Allocate our texture2d object that are holding the target texture
			D3D11Texture2D* texture = new D3D11Texture2D();
			D3D11_TEXTURE2D_DESC d3d_desc;


			d3d_desc.ArraySize = desc.array_size;
			d3d_desc.CPUAccessFlags = 0;
			d3d_desc.MipLevels = desc.mip_count;
			d3d_desc.MiscFlags = 0;
			d3d_desc.Usage = D3D11_USAGE_DEFAULT;

			// TODO: Multisampling on render targets
			d3d_desc.SampleDesc.Count = 1;
			d3d_desc.SampleDesc.Quality = 0;

			d3d_desc.Width = desc.width;
			d3d_desc.Height = desc.height;

			d3d_desc.Format = ToDXGIFormatTypeless(desc.pixel_format);
			Assert(d3d_desc.Format != DXGI_FORMAT_UNKNOWN);

			d3d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

			if (bind_flags & RRenderTarget::BIND_RENDER_TARGET)
			{
				d3d_desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
			}
			else if (bind_flags & RRenderTarget::BIND_DEPTH_STENCIL)
			{
				d3d_desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
			}

			if (bind_flags & RRenderTarget::BIND_UNORDERED_ACCESS)
			{
				d3d_desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
			}

			D3D_VERIFY(device->CreateTexture2D(&d3d_desc, 0, &texture->texture));
			d3d_target->size = d3d_desc.Width * d3d_desc.Height * (image::BitsPerPixel(desc.pixel_format) / 8);

			// Create ShaderResourceView, we create it here since we don't need any SRV for the back buffer
			D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
			srv_desc.Format = ToDXGIFormat(desc.pixel_format, false);
			if (desc.array_size > 1)
			{
				srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
				srv_desc.Texture2DArray.MipLevels = desc.mip_count;
				srv_desc.Texture2DArray.ArraySize = desc.array_size;
				srv_desc.Texture2DArray.FirstArraySlice = 0;
				srv_desc.Texture2DArray.MostDetailedMip = 0;
			}
			else
			{
				srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srv_desc.Texture2D.MipLevels = desc.mip_count;
				srv_desc.Texture2D.MostDetailedMip = 0;
			}

			switch (desc.pixel_format)
			{
			case image::PF_D16:
				srv_desc.Format = DXGI_FORMAT_R16_UNORM;
				break;
			case image::PF_D32F:
				srv_desc.Format = DXGI_FORMAT_R32_FLOAT;
				break;
			case image::PF_D24S8:
				srv_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				break;
			};


			D3D_VERIFY(device->CreateShaderResourceView(texture->texture.Get(), &srv_desc, &d3d_target->srv));

			// DepthStencilView (if needed)
			if (bind_flags & RRenderTarget::BIND_DEPTH_STENCIL)
			{
				D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
				memset(&dsv_desc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

				dsv_desc.Format = ToDXGIFormat(desc.pixel_format, false);
				if (desc.array_size > 1)
				{
					dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
					dsv_desc.Texture2DArray.MipSlice = 0;
					dsv_desc.Texture2DArray.ArraySize = desc.array_size;
					dsv_desc.Texture2DArray.FirstArraySlice = 0;

				}
				else
				{
					dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
					dsv_desc.Texture2D.MipSlice = 0;
				}
				D3D_VERIFY(device->CreateDepthStencilView(texture->texture.Get(), &dsv_desc, &d3d_target->dsv));

				// Specify that the target is a depth stencil
				d3d_target->depth_stencil = true;
			}

			// RenderTargetView (if needed)
			if (bind_flags & RRenderTarget::BIND_RENDER_TARGET)
			{
				D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;
				memset(&rtv_desc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

				rtv_desc.Format = ToDXGIFormat(desc.pixel_format, false);
				if (desc.array_size > 1)
				{
					rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
					rtv_desc.Texture2DArray.MipSlice = 0;
					rtv_desc.Texture2DArray.ArraySize = desc.array_size;
					rtv_desc.Texture2DArray.FirstArraySlice = 0;
				}
				else
				{
					rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
					rtv_desc.Texture2D.MipSlice = 0;
				}
				D3D_VERIFY(device->CreateRenderTargetView(texture->texture.Get(), &rtv_desc, &d3d_target->rtv));
			}

			// Unordered access
			if (bind_flags & RRenderTarget::BIND_UNORDERED_ACCESS)
			{
				D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
				memset(&uav_desc, 0, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));

				uav_desc.Format = ToDXGIFormat(desc.pixel_format, false);
				if (desc.array_size > 1)
				{
					uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
					uav_desc.Texture2DArray.MipSlice = 0;
					uav_desc.Texture2DArray.ArraySize = desc.array_size;
					uav_desc.Texture2DArray.FirstArraySlice = 0;
				}
				else
				{
					uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
					uav_desc.Texture2D.MipSlice = 0;
				}

				device->CreateUnorderedAccessView(texture->texture.Get(), &uav_desc, &d3d_target->uav);
			}

			d3d_target->target = texture;
			break;
		}
		case TextureDesc::TYPE_3D:
			AssertMsg(false, "Not implemented");
			break;
		case TextureDesc::TYPE_CUBE:
			AssertMsg(false, "Not implemented");
			break;
		default:
			Assert(false);
		};

		// Update statistics
		_statistics->render_target_memory += d3d_target->size;
	}
	void D3D11TextureManager::DestroyRenderTarget(D3D11RenderTarget* d3d_target)
	{
		// Release target texture
		switch (d3d_target->target->type)
		{
		case D3D11Resource::TEXTURE2D:
		{
			D3D11Texture2D* tex = (D3D11Texture2D*)d3d_target->target;
			delete tex;
		}
			break;
		default:
			Assert(false);
		};

		// Update statistics
		_statistics->render_target_memory -= d3d_target->size;
		d3d_target->size = 0;
	}

	//-------------------------------------------------------------------------------

	void D3D11TextureManager::CreateBackBuffer(D3D11RenderTarget* d3d_target, IDXGISwapChain* swap_chain, const TextureDesc& desc)
	{
		ComPtr<ID3D11Device> device = _device->GetD3DDevice();

		// First create the target texture
		Assert(desc.type == TextureDesc::TYPE_2D);

		// Allocate our texture2d object that are holding the target texture
		D3D11Texture2D* texture = new D3D11Texture2D();
		D3D11_TEXTURE2D_DESC d3d_desc;

		// If this is a back buffer resource, which is render targets created for swap chains,
		//	then we skip creating a new texture and just use the already existing back buffer
		D3D_VERIFY(swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&texture->texture));

		// Get description for back buffer (For creating the render target view)
		texture->texture->GetDesc(&d3d_desc);

		D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;
		memset(&rtv_desc, 0, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

		rtv_desc.Format = ToDXGIFormat(desc.pixel_format, false);
		Assert(desc.array_size == 1);

		rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtv_desc.Texture2D.MipSlice = 0;

		D3D_VERIFY(device->CreateRenderTargetView(texture->texture.Get(), &rtv_desc, &d3d_target->rtv));

		d3d_target->target = texture;

		// Update statistics
		_statistics->render_target_memory += d3d_target->size;
	}
	void D3D11TextureManager::DestroyBackBuffer(D3D11RenderTarget* d3d_target)
	{
		DestroyRenderTarget(d3d_target);
	}

	//-------------------------------------------------------------------------------

	DXGI_FORMAT D3D11TextureManager::ToDXGIFormat(image::PixelFormat fmt, bool srgb)
	{
		if (srgb)
		{
			switch (fmt)
			{
			case image::PF_R8G8B8A8:
				return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			case image::PF_BC1:
				return DXGI_FORMAT_BC1_UNORM_SRGB;
			case image::PF_BC2:
				return DXGI_FORMAT_BC2_UNORM_SRGB;
			case image::PF_BC3:
				return DXGI_FORMAT_BC3_UNORM_SRGB;
			};
		}
		else
		{
			switch (fmt)
			{
			case image::PF_R8G8B8A8:
				return DXGI_FORMAT_R8G8B8A8_UNORM;
			case image::PF_R16G16B16A16F:
				return DXGI_FORMAT_R16G16B16A16_FLOAT;
			case image::PF_R32G32B32A32F:
				return DXGI_FORMAT_R32G32B32A32_FLOAT;

			case image::PF_R8_UNORM:
				return DXGI_FORMAT_R8_UNORM;
			case image::PF_R16F:
				return DXGI_FORMAT_R16_FLOAT;
			case image::PF_R16_UNORM:
				return DXGI_FORMAT_R16_UNORM;
			case image::PF_R32F:
				return DXGI_FORMAT_R32_FLOAT;

			case image::PF_BC1:
				return DXGI_FORMAT_BC1_UNORM;
			case image::PF_BC2:
				return DXGI_FORMAT_BC2_UNORM;
			case image::PF_BC3:
				return DXGI_FORMAT_BC3_UNORM;
			case image::PF_BC4:
				return DXGI_FORMAT_BC4_UNORM;
			case image::PF_BC5:
				return DXGI_FORMAT_BC5_UNORM;

			case image::PF_R8G8_UNORM:
				return DXGI_FORMAT_R8G8_UNORM;
			case image::PF_R16G16_UNORM:
				return DXGI_FORMAT_R16G16_UNORM;

			case image::PF_D16:
				return DXGI_FORMAT_D16_UNORM;
			case image::PF_D32F:
				return DXGI_FORMAT_D32_FLOAT;
			case image::PF_D24S8:
				return DXGI_FORMAT_D24_UNORM_S8_UINT;
			};
		}




		return DXGI_FORMAT_UNKNOWN;
	}

	DXGI_FORMAT D3D11TextureManager::ToDXGIFormatTypeless(image::PixelFormat fmt)
	{
		switch (fmt)
		{
		case image::PF_R8G8B8A8:
			return DXGI_FORMAT_R8G8B8A8_TYPELESS;
		case image::PF_R16G16B16A16F:
			return DXGI_FORMAT_R16G16B16A16_TYPELESS;
		case image::PF_R32G32B32A32F:
			return DXGI_FORMAT_R32G32B32A32_TYPELESS;

		case image::PF_R8_UNORM:
			return DXGI_FORMAT_R8_TYPELESS;
		case image::PF_R16F:
			return DXGI_FORMAT_R16_TYPELESS;
		case image::PF_R16_UNORM:
			return DXGI_FORMAT_R16_TYPELESS;
		case image::PF_R32F:
			return DXGI_FORMAT_R32_TYPELESS;

		case image::PF_BC1:
			return DXGI_FORMAT_BC1_TYPELESS;
		case image::PF_BC2:
			return DXGI_FORMAT_BC2_TYPELESS;
		case image::PF_BC3:
			return DXGI_FORMAT_BC3_TYPELESS;
		case image::PF_BC4:
			return DXGI_FORMAT_BC4_TYPELESS;
		case image::PF_BC5:
			return DXGI_FORMAT_BC5_TYPELESS;

		case image::PF_R8G8_UNORM:
			return DXGI_FORMAT_R8G8_TYPELESS;
		case image::PF_R16G16_UNORM:
			return DXGI_FORMAT_R16G16_TYPELESS;

		case image::PF_D16:
			return DXGI_FORMAT_R16_TYPELESS;
		case image::PF_D32F:
			return DXGI_FORMAT_R32_TYPELESS;
		case image::PF_D24S8:
			return DXGI_FORMAT_R24G8_TYPELESS;

		};

		return DXGI_FORMAT_UNKNOWN;
	}
	//-------------------------------------------------------------------------------

} // namespace sb

