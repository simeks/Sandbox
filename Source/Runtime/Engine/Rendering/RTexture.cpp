// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "RTexture.h"


namespace sb
{

	//-------------------------------------------------------------------------------

	uint32_t image::BitsPerPixel(PixelFormat fmt)
	{
		switch (fmt)
		{
		case PF_R16G16B16A16F:
			return 64;
		case PF_R32G32B32A32F:
			return 128;

		case PF_R8_UNORM:
			return 8;

		case PF_R16F:
		case PF_R16_UNORM:
		case PF_R8G8_UNORM:
		case PF_D16:
			return 16;

		case PF_R8G8B8A8:
		case PF_R32F:
		case PF_R16G16_UNORM:
		case PF_D32F:
		case PF_D24S8:
			return 32;

		case PF_BC1:
			return 4;

		case PF_BC2:
		case PF_BC3:
		case PF_BC4:
		case PF_BC5:
			return 8;

		case PF_UNKNOWN:
		case PF_NUM_FORMATS:
			Assert(false);
			return 0;
		};

		Assert(false); // Invalid format
		return 0;
	}

	image::PixelFormat image::ParseFormat(const char* fmt)
	{
		if (strcmp(fmt, "R8G8B8A8") == 0)
			return image::PF_R8G8B8A8;
		if (strcmp(fmt, "R16G16B16A16F") == 0)
			return image::PF_R16G16B16A16F;
		if (strcmp(fmt, "R32G32B32A32F") == 0)
			return image::PF_R32G32B32A32F;

		if (strcmp(fmt, "R8G8B8A8") == 0)
			return image::PF_R8_UNORM;
		if (strcmp(fmt, "R8G8B8A8") == 0)
			return image::PF_R16F;
		if (strcmp(fmt, "R8G8B8A8") == 0)
			return image::PF_R16_UNORM;
		if (strcmp(fmt, "R8G8B8A8") == 0)
			return image::PF_R32F;

		if (strcmp(fmt, "BC1") == 0)
			return image::PF_BC1;
		if (strcmp(fmt, "BC2") == 0)
			return image::PF_BC2;
		if (strcmp(fmt, "BC3") == 0)
			return image::PF_BC3;
		if (strcmp(fmt, "BC4") == 0)
			return image::PF_BC4;
		if (strcmp(fmt, "BC5") == 0)
			return image::PF_BC5;

		if (strcmp(fmt, "R8G8") == 0)
			return image::PF_R8G8_UNORM;
		if (strcmp(fmt, "R16G16") == 0)
			return image::PF_R16G16_UNORM;

		if (strcmp(fmt, "D16") == 0)
			return image::PF_D16;
		if (strcmp(fmt, "D32F") == 0)
			return image::PF_D32F;
		if (strcmp(fmt, "D24S8") == 0)
			return image::PF_D24S8;

		return image::PF_UNKNOWN;
	}

	//-------------------------------------------------------------------------------
	RTexture::RTexture() : RenderResource(TEXTURE)
	{
	}

	RTexture::RTexture(TextureDesc& desc) : RenderResource(TEXTURE), _desc(desc)
	{
	}

	const TextureDesc& RTexture::GetDesc() const
	{
		return _desc;
	}
	TextureDesc& RTexture::GetDesc()
	{
		return _desc;
	}
	//-------------------------------------------------------------------------------

} // namespace sb

