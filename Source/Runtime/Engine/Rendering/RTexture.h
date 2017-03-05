// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_RTEXTURE_H__
#define __RENDERING_RTEXTURE_H__

#include "RenderResource.h"
#include "Rendering.h"

namespace sb
{

	namespace image
	{
		enum PixelFormat
		{
			PF_UNKNOWN = 0,
			PF_R8G8B8A8 = 1,
			PF_R16G16B16A16F = 2,
			PF_R32G32B32A32F = 3,

			PF_R8_UNORM = 4,
			PF_R16F = 5,
			PF_R16_UNORM = 6,
			PF_R32F = 7,

			PF_BC1 = 8,
			PF_BC2 = 9,
			PF_BC3 = 10,
			PF_BC4 = 11,
			PF_BC5 = 12,

			PF_R8G8_UNORM = 13,
			PF_R16G16_UNORM = 14,

			PF_D16 = 15,
			PF_D32F = 16,
			PF_D24S8 = 17,


			PF_NUM_FORMATS = 18
		};

		struct Surface
		{
			uint32_t size;
			uint8_t* data;

			Surface() : size(0), data(nullptr) {}
		};

		uint32_t BitsPerPixel(PixelFormat fmt);
		image::PixelFormat ParseFormat(const char* fmt);
	};

	/// Texture description
	struct TextureDesc
	{
		enum Type
		{
			TYPE_2D,
			TYPE_3D,
			TYPE_CUBE
		};
		enum Usage
		{
			STATIC,
			DYNAMIC
		};
		enum Flags
		{
			SRGB = 0x1
		};

		image::PixelFormat pixel_format;
		uint32_t width;
		uint32_t height;
		uint32_t mip_count;
		uint32_t array_size;

		Type type;
		Usage usage;

		uint32_t flags;

		TextureDesc() : pixel_format(image::PF_UNKNOWN), width(0), height(0), mip_count(0), array_size(1), type(TYPE_2D), usage(STATIC), flags(0) {}
	};


	/// Texture render resource
	class RTexture : public RenderResource
	{
	public:
		RTexture();
		RTexture(TextureDesc& desc);

		const TextureDesc& GetDesc() const;
		TextureDesc& GetDesc();

	private:
		TextureDesc _desc;
	};


} // namespace sb


#endif // __RENDERING_RTEXTURE_H__

