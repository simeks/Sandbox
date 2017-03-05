// Copyright 2008-2014 Simon Ekström

#ifndef _MATH_VEC3_H
#define _MATH_VEC3_H

#include "Math.h"


namespace sb
{

	/// @brief Three-dimensional column vector.
	template<typename T> class Vec3
	{
	public:
		T x, y, z;

		Vec3();
		explicit Vec3(T vx, T vy, T vz);
		explicit Vec3(T v);

		/// @brief Returns the length of the vector
		T Length() const;

		/// @brief Normalizes the vector
		void Normalize();

		/// @brief Returns a normalized copy
		Vec3<T> GetNormalized() const;

		/// @brief Returns the cross product between this and the specified vector.
		Vec3<T> Cross(const Vec3<T>& v) const;

		/// @brief Returns the dot product between this and the specified vector.
		T Dot(const Vec3<T>& v) const;

		//-------------------------------------------------------------------------------
		// Operators
		//-------------------------------------------------------------------------------

		Vec3<T> operator - () const;
		Vec3<T> operator * (T k) const;
		Vec3<T> operator / (T k) const;
		Vec3<T>& operator *= (T k);
		Vec3<T>& operator /= (T k);
		Vec3<T>& operator *= (const Vec3<T>& v);
		Vec3<T>& operator /= (const Vec3<T>& v);

		T& operator [] (int i);
		const T& operator [] (int i) const;


		//-------------------------------------------------------------------------------

		static const Vec3<T> ZERO;
		static const Vec3<T> ONE;
		static const Vec3<T> UNIT_X;
		static const Vec3<T> UNIT_Y;
		static const Vec3<T> UNIT_Z;

		//-------------------------------------------------------------------------------
	};

	//-------------------------------------------------------------------------------
	typedef Vec3<float> Vec3f;
	typedef Vec3<double> Vec3d;

	template<typename T> const Vec3<T> Vec3<T>::ZERO(0, 0, 0);
	template<typename T> const Vec3<T> Vec3<T>::ONE(1, 1, 1);
	template<typename T> const Vec3<T> Vec3<T>::UNIT_X(1, 0, 0);
	template<typename T> const Vec3<T> Vec3<T>::UNIT_Y(0, 1, 0);
	template<typename T> const Vec3<T> Vec3<T>::UNIT_Z(0, 0, 1);

	//-------------------------------------------------------------------------------

	template<typename T>
	Vec3<T>::Vec3() : x(0), y(0), z(0)
	{
	}
	template<typename T>
	Vec3<T>::Vec3(T vx, T vy, T vz) : x(vx), y(vy), z(vz)
	{
	}
	template<typename T>
	Vec3<T>::Vec3(T v) : x(v), y(v), z(v)
	{
	}

	template<typename T>
	T Vec3<T>::Length() const
	{
		return math::Sqrt(x*x + y*y + z*z);
	}

	template<typename T>
	void Vec3<T>::Normalize()
	{
		T sum = x*x + y*y + z*z;
		if (sum > FLT_EPSILON)
		{
			T inv_len = math::InvSqrt(sum);
			x *= inv_len; y *= inv_len; z *= inv_len;
		}
	}
	template<typename T>
	Vec3<T> Vec3<T>::GetNormalized() const
	{
		T sum = x*x + y*y + z*z;
		if (sum > FLT_EPSILON)
		{
			T inv_len = math::InvSqrt(sum);
			return *this * inv_len;
		}
		return *this;
	}

	template<typename T>
	Vec3<T> Vec3<T>::Cross(const Vec3<T>& v) const
	{
		// Cross product: [Real-Time Rendering, A.19, page 896]
		//	U x V = (Uy*Vz)i + (Uz*Vx)j + (Ux*Vy)k - (Uz*Vy)i - (Ux*Vz)j - (Uy*Vx)k

		return Vec3<T>(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
	}

	template<typename T>
	T Vec3<T>::Dot(const Vec3<T>& v) const
	{
		// Dot product: [Real-Time Rendering, A.8, page 891]
		//	U dot V = Ux * Vx + Uy * Vy + Uz * Vz + ... 

		return (x * v.x + y * v.y + z * v.z);
	}

	//-------------------------------------------------------------------------------
	// Operators
	//-------------------------------------------------------------------------------

	template<typename T>
	Vec3<T> Vec3<T>::operator - () const
	{
		return Vec3<T>(-x, -y, -z);
	}

	template<typename T>
	Vec3<T> Vec3<T>::operator * (T k) const
	{
		return Vec3<T>(x*k, y*k, z*k);
	}
	template<typename T>
	Vec3<T> Vec3<T>::operator / (T k) const
	{
		k = (T)1.0 / k; return Vec3<T>(x*k, y*k, z*k);
	}
	template<typename T>
	Vec3<T>& Vec3<T>::operator *= (T k)
	{
		x *= k; y *= k; z *= k; return *this;
	}
	template<typename T>
	Vec3<T>& Vec3<T>::operator /= (T k)
	{
		Assert(k != 0);
		k = (T)1.0 / k; x *= k; y *= k; z *= k; return *this;
	}

	template<typename T>
	Vec3<T>& Vec3<T>::operator *= (const Vec3<T>& v)
	{
		x *= v.x; y *= v.y; z *= v.z; return *this;
	}
	template<typename T>
	Vec3<T>& Vec3<T>::operator /= (const Vec3<T>& v)
	{
		Assert(v.x != 0 && v.y != 0 && v.z != 0);
		x /= v.x; y /= v.y; z /= v.z; return *this;
	}

	template<typename T>
	T& Vec3<T>::operator [] (int i)
	{
		Assert(i >= 0 && i < 3);
		return ((T*)this)[i];
	}
	template<typename T>
	const T& Vec3<T>::operator [] (int i) const
	{
		Assert(i >= 0 && i < 3);
		return ((T*)this)[i];
	}

	//-------------------------------------------------------------------------------
	template<typename T>
	Vec3<T> operator * (T f, const Vec3<T>& vec)
	{
		return Vec3<T>((T)(f*vec.x), (T)(f*vec.y), (T)(f*vec.z));
	}
	template<typename T1, typename T2>
	Vec3<T1> operator + (const Vec3<T1>& v0, const Vec3<T2>& v1)
	{
		return Vec3<T1>(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z);
	}
	template<typename T1, typename T2>
	Vec3<T1> operator - (const Vec3<T1>& v0, const Vec3<T2>& v1)
	{
		return Vec3<T1>(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z);
	}

	template<typename T1, typename T2>
	Vec3<T1>& operator += (Vec3<T1>& lhs, const Vec3<T2>& rhs)
	{
		lhs.x += rhs.x; lhs.y += rhs.y; lhs.z += rhs.z;
		return lhs;
	}
	template<typename T1, typename T2>
	Vec3<T1>& operator -= (Vec3<T1>& lhs, const Vec3<T2>& rhs)
	{
		lhs.x -= rhs.x; lhs.y -= rhs.y; lhs.z -= rhs.z;
		return lhs;
	}

	//-------------------------------------------------------------------------------

} // namespace sb




#endif // _MATH_VECTOR_H
