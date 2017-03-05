// Copyright 2008-2014 Simon Ekström

#ifndef _MATH_VEC4_H
#define _MATH_VEC4_H



namespace sb
{

	/// @brief Four-dimensional column vector.
	template<typename T> class Vec4
	{
	public:
		T x, y, z, w;

		Vec4();
		explicit Vec4(T vx, T vy, T vz, T vw);
		explicit Vec4(T v);

		/// @brief Returns the length of the vector
		T Length() const;

		/// @brief Normalizes the vector
		void Normalize();

		/// @brief Returns a normalized copy 
		Vec4<T> GetNormalized() const;

		/// @brief Returns the dot product between this and the specified vector.
		T Dot(const Vec4<T>& vec) const;

		//-------------------------------------------------------------------------------
		// Operators
		//-------------------------------------------------------------------------------

		Vec4<T> operator - () const;
		Vec4<T> operator * (T k) const;
		Vec4<T> operator / (T k) const;
		Vec4<T>& operator *= (T k);
		Vec4<T>& operator /= (T k);

		T& operator [] (int i);
		const T& operator [] (int i) const;

		//-------------------------------------------------------------------------------

		static const Vec4<T> ZERO;
		static const Vec4<T> ONE;
		static const Vec4<T> UNIT_X;
		static const Vec4<T> UNIT_Y;
		static const Vec4<T> UNIT_Z;
		static const Vec4<T> UNIT_W;

		//-------------------------------------------------------------------------------
	};

	//-------------------------------------------------------------------------------

	typedef Vec4<float> Vec4f;
	typedef Vec4<double> Vec4d;

	template<typename T> const Vec4<T> Vec4<T>::ZERO(0, 0, 0, 0);
	template<typename T> const Vec4<T> Vec4<T>::ONE(1, 1, 1, 1);
	template<typename T> const Vec4<T> Vec4<T>::UNIT_X(1, 0, 0, 0);
	template<typename T> const Vec4<T> Vec4<T>::UNIT_Y(0, 1, 0, 0);
	template<typename T> const Vec4<T> Vec4<T>::UNIT_Z(0, 0, 1, 0);
	template<typename T> const Vec4<T> Vec4<T>::UNIT_W(0, 0, 0, 1);

	//-------------------------------------------------------------------------------
	template<typename T>
	Vec4<T>::Vec4() : x(0), y(0), z(0), w(0)
	{
	}

	template<typename T>
	Vec4<T>::Vec4(T vx, T vy, T vz, T vw) : x(vx), y(vy), z(vz), w(vw)
	{
	}

	template<typename T>
	Vec4<T>::Vec4(T v) : x(v), y(v), z(v), w(v)
	{
	}

	template<typename T>
	T Vec4<T>::Length() const
	{
		return math::Sqrt(x*x + y*y + z*z + w*w);
	}

	template<typename T>
	void Vec4<T>::Normalize()
	{
		T sum = x*x + y*y + z*z + w*w;
		if (sum > FLT_EPSILON)
		{
			T inv_len = math::InvSqrt(sum);
			x *= inv_len; y *= inv_len; z *= inv_len; w *= inv_len;
		}
	}

	template<typename T>
	Vec4<T> Vec4<T>::GetNormalized() const
	{
		T sum = x*x + y*y + z*z + w*w;
		if (sum > FLT_EPSILON)
		{
			T inv_len = math::InvSqrt(sum);
			return *this * inv_len;
		}
		return *this;
	}

	template<typename T>
	T Vec4<T>::Dot(const Vec4<T>& vec) const
	{
		return (x * vec.x + y * vec.y + z * vec.z + w * vec.w);
	}

	//-------------------------------------------------------------------------------
	// Operators
	//-------------------------------------------------------------------------------

	template<typename T>
	Vec4<T> Vec4<T>::operator - () const
	{
		return Vec4<T>(-x, -y, -z, -w);
	}

	template<typename T>
	Vec4<T> Vec4<T>::operator * (T k) const
	{
		return Vec4<T>(x*k, y*k, z*k, w*k);
	}
	template<typename T>
	Vec4<T> Vec4<T>::operator / (T k) const
	{
		k = (T)1.0 / k; return Vec4<T>(x*k, y*k, z*k, w*k);
	}
	template<typename T>
	Vec4<T>& Vec4<T>::operator *= (T k)
	{
		x *= k; y *= k; z *= k; w *= k; return *this;
	}
	template<typename T>
	Vec4<T>& Vec4<T>::operator /= (T k)
	{
		k = (T)1.0 / k; x *= k; y *= k; z *= k; w *= k; return *this;
	}

	template<typename T>
	T& Vec4<T>::operator [] (int i)
	{
		Assert(i >= 0 && i < 4);
		return ((T*)this)[i];
	}
	template<typename T>
	const T& Vec4<T>::operator [] (int i) const
	{
		Assert(i >= 0 && i < 4);
		return ((T*)this)[i];
	}


	//-------------------------------------------------------------------------------

	template<typename T1, typename T2>
	Vec4<T1> operator + (const Vec4<T1>& v0, const Vec4<T2>& v1)
	{
		return Vec4<T1>(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z, v0.w + v1.w);
	}
	template<typename T1, typename T2>
	Vec4<T1> operator - (const Vec4<T1>& v0, const Vec4<T2>& v1)
	{
		return Vec4<T1>(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z, v0.w - v1.w);
	}

	template<typename T1, typename T2>
	Vec4<T1>& operator += (Vec4<T1>& lhs, const Vec4<T2>& rhs)
	{
		lhs.x += rhs.x; lhs.y += rhs.y; lhs.z += rhs.z; lhs.w += rhs.w;
		return lhs;
	}
	template<typename T1, typename T2>
	Vec4<T1>& operator -= (Vec4<T1>& lhs, const Vec4<T2>& rhs)
	{
		lhs.x -= rhs.x; lhs.y -= rhs.y; lhs.z -= rhs.z; lhs.w -= rhs.w;
		return lhs;
	}

	//-------------------------------------------------------------------------------

} // namespace sb


#endif // _MATH_VEC4_H
