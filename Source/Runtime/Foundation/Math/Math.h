// Copyright 2008-2014 Simon Ekström

#ifndef _MATH_H
#define _MATH_H

// Constants
#define MATH_PI 3.14159265358979323846
#define MATH_TWO_PI (MATH_PI * 2.0)
#define MATH_HALF_PI (MATH_PI * 0.5)
#define MATH_DEG_TO_RAD ((float)MATH_PI / 180.0f)


namespace sb
{

	/// @brief Math utilities
	namespace math
	{


		float	Sqrt(float x);			// Square root
		float	InvSqrt(float x);			// Inverse square root

		float	Sin(float a);				// Sine
		float	Cos(float a);				// Cosine
		float	Tan(float a);				// Tangent
		void	SinCos(float a, float& s, float& c);	// Sine and cosine

		float	ASin(float a);			// Arc sine
		float	ACos(float a);			// Arc cosine
		float	ATan(float a);			// Arc tangent

		float	Pow(float x, float y);	// x raised to the power of y
		float	Exp(float x);				// e raised to the power of x
		float	Log(float x);				// Natural logarithm

		int		Abs(int x);				// Returns the absolute value for the integer x
		float	Fabs(float x);			// Returns the absolute value for the value x
		float	Floor(float x);			// Returns the largest integer that is <= the given value x
		float	Ceil(float x);			// Returns the smallest integer that is >= the given value x
		int		Ftoi(float x);			// Float to integer conversion
		float	Round(float x);

		int		ClampInt(int min, int max, int value);		// Clamps the given integer value
		float	ClampFloat(float min, float max, float value); // Clamps the given float value

		float	AngleNormalize360(float angle); // Normalizes the given angle to the range [0 <= angle < 360]
		float	AngleNormalize180(float angle); // Normalizes the given angle to the range [-180 < angle <= 180]

		float	AngleNormalize2PI(float angle); // Normalizes the given angle to the range [0 <= angle < 2*PI]
		float	AngleNormalizePI(float angle); // Normalizes the given angle to the range [-PI < angle <= PI]

	};
	INLINE float math::Sqrt(float x)
	{
		return sqrtf(x);
	}
	INLINE float math::InvSqrt(float x)
	{
		return 1.0f / sqrtf(x);
	}
	INLINE float math::Sin(float a)
	{
		return sinf(a);
	}
	INLINE float math::Cos(float a)
	{
		return cosf(a);
	}
	INLINE float math::Tan(float a)
	{
		return tanf(a);
	}
	INLINE void math::SinCos(float a, float& s, float& c)
	{
		s = sinf(a);
		c = cosf(a);
	}
	INLINE float math::ASin(float a)
	{
		if (a <= -1.0f)
		{
			return (float)-MATH_HALF_PI;
		}
		if (a >= 1.0f)
		{
			return (float)MATH_HALF_PI;
		}
		return asinf(a);
	}
	INLINE float math::ACos(float a)
	{
		if (a <= -1.0f)
		{
			return (float)MATH_PI;
		}
		if (a >= 1.0f)
		{
			return 0.0f;
		}
		return acosf(a);
	}
	INLINE float math::ATan(float a)
	{
		return atanf(a);
	}
	INLINE float math::Pow(float x, float y)
	{
		return powf(x, y);
	}
	INLINE float math::Exp(float x)
	{
		return expf(x);
	}
	INLINE float math::Log(float x)
	{
		return logf(x);
	}

	INLINE int math::Abs(int x)
	{
		return abs(x);
	}
	INLINE float math::Fabs(float x)
	{
		return fabs(x);
	}
	INLINE float math::Floor(float x)
	{
		return floorf(x);
	}
	INLINE float math::Ceil(float x)
	{
		return ceilf(x);
	}
	INLINE int math::Ftoi(float x)
	{
		return (int)x;
	}
	INLINE float math::Round(float x)
	{
		return floor(x + 0.5f);
	}

	INLINE int math::ClampInt(int min, int max, int value)
	{
		if (value < min)
		{
			return min;
		}
		if (value > max)
		{
			return max;
		}
		return value;
	}
	INLINE float math::ClampFloat(float min, float max, float value)
	{
		if (value < min)
		{
			return min;
		}
		if (value > max)
		{
			return max;
		}
		return value;
	}

	INLINE float math::AngleNormalize360(float angle)
	{
		if ((angle >= 360.0f) || (angle < 0.0f))
		{
			angle -= floorf(angle / 360.0f) * 360.0f;
		}
		return angle;
	}
	INLINE float math::AngleNormalize180(float angle)
	{
		angle = AngleNormalize360(angle);
		if (angle > 180.0f)
		{
			angle -= 360.0f;
		}
		return angle;
	}

	INLINE float math::AngleNormalize2PI(float angle)
	{
		if ((angle >= MATH_TWO_PI) || (angle < 0.0f))
		{
			angle -= floorf(angle / (float)MATH_TWO_PI) * (float)MATH_TWO_PI;
		}
		return angle;
	}
	INLINE float math::AngleNormalizePI(float angle)
	{
		angle = AngleNormalize2PI(angle);
		if (angle > MATH_PI)
		{
			angle -= (float)MATH_TWO_PI;
		}
		return angle;
	}

} // namespace sb


#endif // _MATH_H
