// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_CULLING_H__
#define __RENDERING_CULLING_H__

#include <Foundation/Math/Matrix4x4.h>
#include <Foundation/Math/AABB.h>

namespace sb
{

	namespace culling
	{
		enum Flags
		{
			VISIBLE = 0x1,
			ALWAYS_VISIBLE = 0x2
		};
	};

	struct CullableObject
	{
		Mat4x4f world;
		AABB aabb; // AABB in local space (TODO: Store AABB in world space here, no need to recalculate each frame)
		uint32_t flags;

		uint32_t type;
		uint32_t handle;
	};

} // namespace sb

#endif // __RENDERING_CULLING_H__
