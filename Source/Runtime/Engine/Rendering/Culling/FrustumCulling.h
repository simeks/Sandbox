// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_FRUSTUM_CULLING_H__
#define __RENDERING_FRUSTUM_CULLING_H__

#include "Culling.h"

#include <Foundation/Math/Plane.h>

namespace sb
{

	class TaskScheduler;

	namespace culling
	{
		enum { PLANE_COUNT = 6 };

		void FrustumCull(TaskScheduler* scheduler, const CullableObject* objects, uint8_t* visibility_flags, uint32_t num, const Planef* frustum_planes);
	};

} // namespace sb

#endif // __RENDERING_FRUSTUM_CULLING_H__
