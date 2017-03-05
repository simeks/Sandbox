// Copyright 2008-2014 Simon Ekström

#ifndef _MATH_AABB_H
#define _MATH_AABB_H

#include <Foundation/Math/Matrix4x4.h>
#include <Foundation/Math/Vec3.h>



namespace sb
{

	class AABB
	{
	public:
		Vec3f min;
		Vec3f max;

		AABB() : min(Vec3f::ZERO), max(Vec3f::ZERO)
		{
		}
		AABB(Vec3f vec_min, Vec3f vec_max)
		{
			min = vec_min; max = vec_max;
		}
		AABB(float radius)
		{
			max = Vec3f(radius); min = -max;
		}

		void Merge(const AABB& rhs)
		{
			min.x = Min(min.x, rhs.min.x);
			min.y = Min(min.y, rhs.min.y);
			min.z = Min(min.z, rhs.min.z);

			max.x = Max(max.x, rhs.max.x);
			max.y = Max(max.y, rhs.max.y);
			max.z = Max(max.z, rhs.max.z);
		}
		void Merge(const Vec3f& rhs)
		{
			min.x = Min(min.x, rhs.x);
			min.y = Min(min.y, rhs.y);
			min.z = Min(min.z, rhs.z);

			max.x = Max(max.x, rhs.x);
			max.y = Max(max.y, rhs.y);
			max.z = Max(max.z, rhs.z);
		}

		void Transform(const Mat4x4f& mat)
		{
			Vec3f min_a = min, min_b;
			Vec3f max_a = max, max_b;

			min_b = max_b = mat.GetTranslation();

			for (uint32_t i = 0; i < 3; ++i)
			{
				for (uint32_t j = 0; j < 3; ++j)
				{
					float x = min_a[j] * mat(j, i);
					float y = max_a[j] * mat(j, i);

					min_b[i] += Min(x, y);
					max_b[i] += Max(x, y);
				}
			}

			min = min_b;
			max = max_b;
		}


		//-------------------------------------------------------------------------------

	};

} // namespace sb


#endif // _MATH_AABB_H