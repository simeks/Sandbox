// Copyright 2008-2014 Simon Ekström

#ifndef __MATH_FRUSTUM_H__
#define __MATH_FRUSTUM_H__

#include "Plane.h"
#include "Matrix4x4.h"
#include "Vec3.h"


namespace sb
{

	struct Frustum
	{
		enum
		{
			PLANE_LEFT,
			PLANE_RIGHT,
			PLANE_NEAR,
			PLANE_FAR,
			PLANE_BOTTOM,
			PLANE_TOP,

			NUM_PLANES // Number of planes
		};

		Frustum() {}

		Planef planes[NUM_PLANES];

	};



	namespace frustum
	{
		/// Calculates a frustum from a view projection matrix
		void CalculateFrustum(const Mat4x4f& view_proj, Frustum& frustum);

		/// Calculates the corners of a frustum
		void CalculateFrustumCorners(const Frustum& frustum, Vec3f* corners);

	} // namespace frustum

} // namespace sb


#endif // __MATH_FRUSTUM_H__
