// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_H__
#define __RENDERING_H__

#define MAX_MULTIPLE_RENDER_TARGETS 8

namespace sb
{

	struct Viewport
	{
		float x;
		float y;
		float width;
		float height;
		float min_depth;
		float max_depth;
	};

	struct ScissorRect
	{
		float x;
		float y;
		float width;
		float height;
	};


	namespace rendering
	{

		enum ClearFlag
		{
			CLEAR_COLOR = 0x1,
			CLEAR_DEPTH = 0x2,
			CLEAR_STENCIL = 0x4
		};

	}; // namespace rendering


	namespace render_sorting
	{
		// 64-bit sort key:
		// LLLLLLLL VVVVVVVV UUUUUUUU UUUUUUUU UUUUUUUU PPPP---I DDDDDDDD DDDDDDDD 
		//	[63:56] L : Layer index
		//	[55:48] V : Render view index
		//	[47:24] U : User data (for render views)
		//	[23:19] P : Shader pass
		//	[16:16] I : Instance bit
		//	[15:0]	D : Depth

		enum BitOffset
		{
			LAYER_BIT = 56,
			VIEW_BIT = 48,
			USER_DATA_BIT = 24,
			SHADER_PASS_BIT = 19,
			INSTANCE_BIT = 16,
			DEPTH_BIT = 0
		};
		enum NumBits
		{
			LAYER_NUM_BITS = 8,
			VIEW_NUM_BITS = 8,
			USER_DATA_NUM_BITS = 24,
			SHADER_PASS_NUM_BITS = 4,
			INSTANCE_NUM_BITS = 1,
			DEPTH_NUM_BITS = 16
		};

		enum DepthSort
		{
			FRONT_TO_BACK,
			BACK_TO_FRONT
		};

	}; // namespace render_sorting

} // namespace sb


#endif // __RENDERING_H__
