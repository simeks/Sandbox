// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "DDSImage.h"
#include "RTexture.h"

#include <Foundation/IO/Stream.h>

#define DDS_MAGIC 0x20534444 // "DDS "

#define DDS_FOURCC      0x00000004  // DDPF_FOURCC
#define DDS_RGB         0x00000040  // DDPF_RGB
#define DDS_RGBA        0x00000041  // DDPF_RGB | DDPF_ALPHAPIXELS
#define DDS_LUMINANCE   0x00020000  // DDPF_LUMINANCE
#define DDS_ALPHA       0x00000002  // DDPF_ALPHA


#define DDS_HEADER_FLAGS_TEXTURE        0x00001007  // DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT 
#define DDS_HEADER_FLAGS_MIPMAP         0x00020000  // DDSD_MIPMAPCOUNT
#define DDS_HEADER_FLAGS_VOLUME         0x00800000  // DDSD_DEPTH
#define DDS_HEADER_FLAGS_PITCH          0x00000008  // DDSD_PITCH
#define DDS_HEADER_FLAGS_LINEARSIZE     0x00080000  // DDSD_LINEARSIZE

#define DDS_SURFACE_FLAGS_TEXTURE 0x00001000 // DDSCAPS_TEXTURE
#define DDS_SURFACE_FLAGS_MIPMAP  0x00400008 // DDSCAPS_COMPLEX | DDSCAPS_MIPMAP
#define DDS_SURFACE_FLAGS_CUBEMAP 0x00000008 // DDSCAPS_COMPLEX

#define DDS_CUBEMAP_POSITIVEX 0x00000600 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX
#define DDS_CUBEMAP_NEGATIVEX 0x00000a00 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEX
#define DDS_CUBEMAP_POSITIVEY 0x00001200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEY
#define DDS_CUBEMAP_NEGATIVEY 0x00002200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEY
#define DDS_CUBEMAP_POSITIVEZ 0x00004200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEZ
#define DDS_CUBEMAP_NEGATIVEZ 0x00008200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEZ

#define DDS_CUBEMAP_ALLFACES ( DDS_CUBEMAP_POSITIVEX | DDS_CUBEMAP_NEGATIVEX |\
	DDS_CUBEMAP_POSITIVEY | DDS_CUBEMAP_NEGATIVEY | \
	DDS_CUBEMAP_POSITIVEZ | DDS_CUBEMAP_NEGATIVEZ)

#define DDS_FLAGS_VOLUME 0x00200000 // DDSCAPS2_VOLUME




#define	MAX_MIP_COUNT	( 15 ) // See D3D11_REQ_MIP_LEVELS	

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
	((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) | \
	((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24))
#endif /* defined(MAKEFOURCC) */


#define ISBITMASK( r,g,b,a ) ( ddpf.dwRBitMask == r && ddpf.dwGBitMask == g && ddpf.dwBBitMask == b && ddpf.dwABitMask == a )


namespace sb
{

	namespace dds_image
	{
		const DDS_PIXELFORMAT DDSPF_DXT1 =
		{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D', 'X', 'T', '1'), 0, 0, 0, 0, 0 };

		const DDS_PIXELFORMAT DDSPF_DXT2 =
		{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D', 'X', 'T', '2'), 0, 0, 0, 0, 0 };

		const DDS_PIXELFORMAT DDSPF_DXT3 =
		{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D', 'X', 'T', '3'), 0, 0, 0, 0, 0 };

		const DDS_PIXELFORMAT DDSPF_DXT4 =
		{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D', 'X', 'T', '4'), 0, 0, 0, 0, 0 };

		const DDS_PIXELFORMAT DDSPF_DXT5 =
		{ sizeof(DDS_PIXELFORMAT), DDS_FOURCC, MAKEFOURCC('D', 'X', 'T', '5'), 0, 0, 0, 0, 0 };

		const DDS_PIXELFORMAT DDSPF_A8R8G8B8 =
		{ sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 };

		const DDS_PIXELFORMAT DDSPF_A1R5G5B5 =
		{ sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 16, 0x00007c00, 0x000003e0, 0x0000001f, 0x00008000 };

		const DDS_PIXELFORMAT DDSPF_A4R4G4B4 =
		{ sizeof(DDS_PIXELFORMAT), DDS_RGBA, 0, 16, 0x00000f00, 0x000000f0, 0x0000000f, 0x0000f000 };

		const DDS_PIXELFORMAT DDSPF_R8G8B8 =
		{ sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000 };

		const DDS_PIXELFORMAT DDSPF_R5G6B5 =
		{ sizeof(DDS_PIXELFORMAT), DDS_RGB, 0, 16, 0x0000f800, 0x000007e0, 0x0000001f, 0x00000000 };

	};

	namespace
	{
		void GetSurfaceInfo(uint32_t width, uint32_t height, image::PixelFormat fmt, uint32_t& num_bytes, uint32_t& row_bytes, uint32_t& row_count)
		{
			num_bytes = 0;
			row_bytes = 0;
			row_count = 0;

			bool bc = true;
			int num_bytes_per_block = 16;
			switch (fmt)
			{
			case image::PF_BC1:
			case image::PF_BC4:
				num_bytes_per_block = 8;
				break;
			case image::PF_BC2:
			case image::PF_BC3:
			case image::PF_BC5:
				break;

			default:
				bc = false;
				break;
			};

			if (bc)
			{
				int blocks_wide = 0;
				if (width > 0)
					blocks_wide = Max<uint32_t>(1, width / 4);
				int blocks_high = 0;
				if (height > 0)
					blocks_high = Max<uint32_t>(1, height / 4);

				row_bytes = blocks_wide * num_bytes_per_block;
				row_count = blocks_high;
			}
			else
			{
				uint32_t bpp = image::BitsPerPixel(fmt);
				row_bytes = (width * bpp + 7) / 8;
				row_count = height;
			}
			num_bytes = row_bytes * row_count;
		};
	};

	void dds_image::Load(Stream& stream, TextureDesc& desc, vector<image::Surface>& surfaces)
	{
		DWORD magic_number;
		stream.Read(&magic_number, sizeof(DWORD));

		Assert(magic_number == DDS_MAGIC);

		DDS_HEADER header;
		stream.Read(&header, sizeof(DDS_HEADER));

		Assert(header.dwSize == sizeof(DDS_HEADER));
		Assert(header.ddspf.dwSize == sizeof(DDS_PIXELFORMAT));

		Assert(header.ddspf.dwFourCC != MAKEFOURCC('D', 'X', '1', '0')); // DX10 extension not supported

		desc.usage = TextureDesc::STATIC;
		desc.width = header.dwWidth;
		desc.height = header.dwHeight;
		desc.mip_count = header.dwMipMapCount;
		desc.type = TextureDesc::TYPE_2D;
		if (desc.mip_count == 0)
			desc.mip_count = 1;

		Assert(desc.mip_count <= MAX_MIP_COUNT);

		desc.pixel_format = GetFormat(header.ddspf);
		Assert(desc.pixel_format != image::PF_UNKNOWN); // Format not supported

		uint32_t num_faces = 1;
		if (header.dwCubemapFlags != 0)
		{
			Assert(header.dwCubemapFlags & DDS_CUBEMAP_ALLFACES); // We only support cubemaps with all faces currently.

			desc.type = TextureDesc::TYPE_CUBE;
			num_faces = 6;
		}

		// TODO: Implement volumes
		Assert(!(header.dwHeaderFlags & DDS_HEADER_FLAGS_VOLUME)); // Not supported

		desc.array_size = num_faces;

		surfaces.clear();

		uint32_t row_bytes, row_count;
		for (uint32_t i = 0; i < num_faces; ++i)
		{
			uint32_t w = desc.width;
			uint32_t h = desc.height;
			for (uint32_t m = 0; m < desc.mip_count; ++m)
			{
				image::Surface surface;
				surface.size = 0;

				GetSurfaceInfo(w, h, desc.pixel_format, surface.size, row_bytes, row_count);
				surface.data = (uint8_t*)memory::Malloc(surface.size);
				stream.Read(surface.data, surface.size);

				surfaces.push_back(surface);

				if (w > 1)
					w = w >> 1;
				if (h > 1)
					h = h >> 1;
			}
		}

	}

	void dds_image::Save(Stream&, const TextureDesc&, const vector<image::Surface>&)
	{
		Assert(false); // Not implemented
	}

	image::PixelFormat dds_image::GetFormat(const DDS_PIXELFORMAT& ddpf)
	{
		if (ddpf.dwFlags & DDS_RGB)
		{
			switch (ddpf.dwRGBBitCount)
			{
			case 32:
				// DXGI_FORMAT_B8G8R8A8_UNORM_SRGB & DXGI_FORMAT_B8G8R8X8_UNORM_SRGB should be
				// written using the DX10 extended header instead since these formats require
				// DXGI 1.1
				//
				// This code will use the fallback to swizzle RGB to BGR in memory for standard
				// DDS files which works on 10 and 10.1 devices with WDDM 1.0 drivers
				//
				// NOTE: We don't use DXGI_FORMAT_B8G8R8X8_UNORM or DXGI_FORMAT_B8G8R8X8_UNORM
				// here because they were defined for DXGI 1.0 but were not required for D3D10/10.1

				if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
					return image::PF_R8G8B8A8; //DXGI_FORMAT_R8G8B8A8_UNORM;
				if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000))
					return image::PF_R8G8B8A8; // No D3DFMT_X8B8G8R8 in DXGI

				//// Note that many common DDS reader/writers swap the
				//// the RED/BLUE masks for 10:10:10:2 formats. We assumme
				//// below that the 'correct' header mask is being used. The
				//// more robust solution is to use the 'DX10' header extension and
				//// specify the DXGI_FORMAT_R10G10B10A2_UNORM format directly
				//if( ISBITMASK(0x000003ff,0x000ffc00,0x3ff00000,0xc0000000) )
				//    return DXGI_FORMAT_R10G10B10A2_UNORM;

				if (ISBITMASK(0x0000ffff, 0xffff0000, 0x00000000, 0x00000000))
					return image::PF_R16G16_UNORM;//DXGI_FORMAT_R16G16_UNORM;

				if (ISBITMASK(0xffffffff, 0x00000000, 0x00000000, 0x00000000))
					// Only 32-bit color channel format in D3D9 was R32F
					return image::PF_R32F;//DXGI_FORMAT_R32_FLOAT; // D3DX writes this out as a FourCC of 114
				break;

			case 24:
				// No 24bpp DXGI formats
				break;

			case 16:
				// 5:5:5 & 5:6:5 formats are defined for DXGI, but are deprecated for D3D10+

				//if( ISBITMASK(0x0000f800,0x000007e0,0x0000001f,0x00000000) )
				//    return DXGI_FORMAT_B5G6R5_UNORM;
				//if( ISBITMASK(0x00007c00,0x000003e0,0x0000001f,0x00008000) )
				//    return DXGI_FORMAT_B5G5R5A1_UNORM;
				//if( ISBITMASK(0x00007c00,0x000003e0,0x0000001f,0x00000000) )
				//    return DXGI_FORMAT_B5G5R5A1_UNORM; // No D3DFMT_X1R5G5B5 in DXGI

				// No 4bpp or 3:3:2 DXGI formats
				break;
			}
		}
		else if (ddpf.dwFlags & DDS_LUMINANCE)
		{
			if (8 == ddpf.dwRGBBitCount)
			{
				if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x00000000))
					return image::PF_R8_UNORM; //DXGI_FORMAT_R8_UNORM; // D3DX10/11 writes this out as DX10 extension

				// No 4bpp DXGI formats
			}

			if (16 == ddpf.dwRGBBitCount)
			{
				if (ISBITMASK(0x0000ffff, 0x00000000, 0x00000000, 0x00000000))
					return image::PF_R16_UNORM; //DXGI_FORMAT_R16_UNORM; // D3DX10/11 writes this out as DX10 extension
				if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x0000ff00))
					return image::PF_R8G8_UNORM; //DXGI_FORMAT_R8G8_UNORM; // D3DX10/11 writes this out as DX10 extension
			}
		}
		else if (ddpf.dwFlags & DDS_FOURCC)
		{
			if (MAKEFOURCC('D', 'X', 'T', '1') == ddpf.dwFourCC)
				return image::PF_BC1; //DXGI_FORMAT_BC1_UNORM;
			if (MAKEFOURCC('D', 'X', 'T', '3') == ddpf.dwFourCC)
				return image::PF_BC2; //DXGI_FORMAT_BC2_UNORM;
			if (MAKEFOURCC('D', 'X', 'T', '5') == ddpf.dwFourCC)
				return image::PF_BC3; //DXGI_FORMAT_BC3_UNORM;

			if (MAKEFOURCC('B', 'C', '4', 'U') == ddpf.dwFourCC)
				return image::PF_BC4; //DXGI_FORMAT_BC4_UNORM;
			if (MAKEFOURCC('B', 'C', '4', 'S') == ddpf.dwFourCC)
				return image::PF_BC4; //DXGI_FORMAT_BC4_SNORM;

			if (MAKEFOURCC('A', 'T', 'I', '2') == ddpf.dwFourCC)
				return image::PF_BC5; //DXGI_FORMAT_BC5_UNORM;
			if (MAKEFOURCC('B', 'C', '5', 'S') == ddpf.dwFourCC)
				return image::PF_BC5; //DXGI_FORMAT_BC5_SNORM;

		}
		return image::PF_UNKNOWN;
	}

} // namespace sb

