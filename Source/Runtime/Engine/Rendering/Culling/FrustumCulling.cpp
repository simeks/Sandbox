// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "FrustumCulling.h"

#include <Foundation/Math/Vec3.h>
#include <Foundation/Profiler/Profiler.h>
#include <Foundation/Thread/TaskScheduler.h>

namespace sb
{

	namespace culling
	{

		struct FrustumCullData
		{
			const CullableObject* objects;
			uint8_t* visibility;
			uint32_t num;

			const Planef* frustum_planes;
			const Planef* abs_frustum_planes;
		};

		void FrustumCullJob(void* data, const Range& range);

	};

	void culling::FrustumCullJob(void* data, const Range& range)
	{
		// Real-Time Rendering, 16.10 - Plane/Box intersection

		PROFILER_SCOPE("FrustumCullJob");

		FrustumCullData* cull_data = (FrustumCullData*)data;
		AABB aabb;

		Vec3f size;
		Vec3f center;
		for (int i = range.begin; i < range.end; ++i)
		{
			cull_data->visibility[i] = 1;

			if (cull_data->objects[i].flags & culling::ALWAYS_VISIBLE)
				continue;

			aabb = cull_data->objects[i].aabb;
			aabb.Transform(cull_data->objects[i].world);

			center = (aabb.min + aabb.max) * 0.5f;
			size = (aabb.max - aabb.min) * 0.5f;

			for (uint32_t p = 0; p < PLANE_COUNT; ++p)
			{
				const Planef& plane = cull_data->frustum_planes[p];
				const Planef& abs_plane = cull_data->abs_frustum_planes[p];

				float d = center.x * plane.n.x +
					center.y * plane.n.y +
					center.z * plane.n.z;

				float r = size.x * abs_plane.n.x +
					size.y * abs_plane.n.y +
					size.z * abs_plane.n.z;

				float d_p_r = d + r;
				if (d_p_r < -plane.d)
				{
					cull_data->visibility[i] = 0;
					break;
				}

				//float d_m_r = d - r + plane.d;
				//if(d_m_r < 0)
				//{
				//	// Intersect
				//}
			}
		}
	}

	void culling::FrustumCull(TaskScheduler* scheduler, const CullableObject* objects, uint8_t* visibility_flags, uint32_t num, const Planef* frustum_planes)
	{
		FrustumCullData data;
		data.objects = objects;
		data.visibility = visibility_flags;
		data.num = num;
		data.frustum_planes = frustum_planes;

		Planef abs_planes[PLANE_COUNT];

		// Prepare planes
		for (uint32_t i = 0; i < PLANE_COUNT; ++i)
		{
			abs_planes[i].n.x = math::Fabs(frustum_planes[i].n.x);
			abs_planes[i].n.y = math::Fabs(frustum_planes[i].n.y);
			abs_planes[i].n.z = math::Fabs(frustum_planes[i].n.z);
		}
		data.abs_frustum_planes = abs_planes;

		if (num != 0)
			scheduling::ParallelFor(scheduler, FrustumCullJob, &data, Range(0, num));
	}

} // namespace sb
