// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Frustum.h"


namespace sb
{

	void frustum::CalculateFrustum(const Mat4x4f& view_proj, Frustum& frustum)
	{
		// Real-Time Rendering, 16.14.1 - Frustum Plane Extraction

		Mat4x4f tmp = view_proj;

		frustum.planes[Frustum::PLANE_LEFT].n.x = tmp.m30 + tmp.m00;
		frustum.planes[Frustum::PLANE_LEFT].n.y = tmp.m31 + tmp.m01;
		frustum.planes[Frustum::PLANE_LEFT].n.z = tmp.m32 + tmp.m02;
		frustum.planes[Frustum::PLANE_LEFT].d = tmp.m33 + tmp.m03;

		frustum.planes[Frustum::PLANE_RIGHT].n.x = tmp.m30 - tmp.m00;
		frustum.planes[Frustum::PLANE_RIGHT].n.y = tmp.m31 - tmp.m01;
		frustum.planes[Frustum::PLANE_RIGHT].n.z = tmp.m32 - tmp.m02;
		frustum.planes[Frustum::PLANE_RIGHT].d = tmp.m33 - tmp.m03;

		frustum.planes[Frustum::PLANE_NEAR].n.x = tmp.m20;
		frustum.planes[Frustum::PLANE_NEAR].n.y = tmp.m21;
		frustum.planes[Frustum::PLANE_NEAR].n.z = tmp.m22;
		frustum.planes[Frustum::PLANE_NEAR].d = tmp.m23;

		frustum.planes[Frustum::PLANE_FAR].n.x = tmp.m30 - tmp.m20;
		frustum.planes[Frustum::PLANE_FAR].n.y = tmp.m31 - tmp.m21;
		frustum.planes[Frustum::PLANE_FAR].n.z = tmp.m32 - tmp.m22;
		frustum.planes[Frustum::PLANE_FAR].d = tmp.m33 - tmp.m23;

		frustum.planes[Frustum::PLANE_TOP].n.x = tmp.m30 + tmp.m10;
		frustum.planes[Frustum::PLANE_TOP].n.y = tmp.m31 + tmp.m11;
		frustum.planes[Frustum::PLANE_TOP].n.z = tmp.m32 + tmp.m12;
		frustum.planes[Frustum::PLANE_TOP].d = tmp.m33 + tmp.m13;

		frustum.planes[Frustum::PLANE_BOTTOM].n.x = tmp.m30 - tmp.m10;
		frustum.planes[Frustum::PLANE_BOTTOM].n.y = tmp.m31 - tmp.m11;
		frustum.planes[Frustum::PLANE_BOTTOM].n.z = tmp.m32 - tmp.m12;
		frustum.planes[Frustum::PLANE_BOTTOM].d = tmp.m33 - tmp.m13;


		for (uint32_t i = 0; i < Frustum::NUM_PLANES; ++i)
		{
			frustum.planes[i].Normalize();
		}
	}

} // namespace sb

