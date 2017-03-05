// Copyright 2008-2014 Simon Ekström

#ifndef _MATH_SPHERE_H
#define _MATH_SPHERE_H

#include "Vec3.h"


namespace sb
{

	class Sphere
	{
	public:
		Vec3f	center;
		float	radius;

		Sphere() : center(Vec3f::ZERO), radius(0.0f) { }
		Sphere(const Vec3f& pt)
		{
			center = pt;
			radius = 0.0f;
		}
		Sphere(const Vec3f& pt, const float r)
		{
			center = pt;
			radius = r;
		}


	};

} // namespace sb



#endif // _MATH_SPHERE_H
