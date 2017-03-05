// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_RENDERBLOCK_H__
#define __RENDERING_RENDERBLOCK_H__

#include "RVertexBuffer.h"
#include "RIndexBuffer.h"
#include "RVertexDeclaration.h"

namespace sb
{

	struct DrawCall
	{
		enum PrimitiveType
		{
			PRIMITIVE_POINT_LIST,
			PRIMITIVE_LINE_LIST,
			PRIMITIVE_LINE_STRIP,
			PRIMITIVE_TRIANGLELIST,
			PRIMITIVE_TRIANGLESTRIP
		};

		PrimitiveType prim_type;

		uint32_t vertex_offset;
		uint32_t vertex_count;
		uint32_t index_offset; ///< Offset of first index to read (in number of bytes)
		uint32_t index_count; ///< Number of indices to read

		/// Number of instances to count, only valid when shader supports instancing. 
		///	If instance_count = 0 the renderer automatically merges instances.
		uint32_t instance_count;

		DrawCall() : prim_type(PRIMITIVE_POINT_LIST), vertex_offset(0), vertex_count(0), index_offset(0), index_count(0), instance_count(0) { }
	};

	/// Block holding all information required for a render call.
	struct RenderBlock
	{
		DrawCall draw_call;

		RenderResource vertex_buffer;
		RenderResource index_buffer;
		RenderResource vertex_declaration;

		RenderBlock() {}
	};

	/// Block for dispatching compute shaders.
	struct ComputeCall
	{
		uint32_t thread_group_count_x;
		uint32_t thread_group_count_y;
		uint32_t thread_group_count_z;
	};

} // namespace sb

#endif // __RENDERING_RENDERBLOCK_H__

