// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_DDSIMAGE_H__
#define __RENDERING_DDSIMAGE_H__

#include "RTexture.h"

namespace sb
{

	class Stream;

	namespace dds_image
	{
		struct DDS_PIXELFORMAT
		{
			DWORD dwSize;
			DWORD dwFlags;
			DWORD dwFourCC;
			DWORD dwRGBBitCount;
			DWORD dwRBitMask;
			DWORD dwGBitMask;
			DWORD dwBBitMask;
			DWORD dwABitMask;
		};

		struct DDS_HEADER
		{
			DWORD dwSize;
			DWORD dwHeaderFlags;
			DWORD dwHeight;
			DWORD dwWidth;
			DWORD dwPitchOrLinearSize;
			DWORD dwDepth; // only if DDS_HEADER_FLAGS_VOLUME is set in dwHeaderFlags
			DWORD dwMipMapCount;
			DWORD dwReserved1[11];
			DDS_PIXELFORMAT ddspf;
			DWORD dwSurfaceFlags;
			DWORD dwCubemapFlags;
			DWORD dwReserved2[3];
		};

		/// Loads a dds image from a data stream and stores it in the specified texture object.
		void Load(Stream& stream, TextureDesc& desc, vector<image::Surface>& surfaces);

		/// Writes the specified texture as a dds image to the specified stream.
		void Save(Stream& stream, const TextureDesc& desc, const vector<image::Surface>& surfaces);

		/// Converts to PixelFormat from the specified DDS_PIXELFORMAT
		image::PixelFormat GetFormat(const DDS_PIXELFORMAT& ddpf);

	};

} // namespace sb


#endif // __RENDERING_DDSIMAGE_H__
