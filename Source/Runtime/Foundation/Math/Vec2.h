// Copyright 2008-2014 Simon Ekström

#ifndef _MATH_VEC2_H
#define _MATH_VEC2_H


namespace sb
{

	/// @brief Two-dimensional column vector.
	template<typename T> class Vec2
	{
	public:
		T x, y;

		Vec2();
		explicit Vec2(T vx, T vy);
		explicit Vec2(T v);

		/// @brief Normalizes the vector
		void Normalize();

		/// @brief Returns a normalized copy
		Vec2<T> GetNormalized() const;

		/// @brief Returns the length of the vector
		T Length() const;

		/// @brief Returns the dot product between this and the specified vector
		T Dot(const Vec2<T>& vec) const;

		//-------------------------------------------------------------------------------
		// Operators
		//-------------------------------------------------------------------------------

		Vec2<T> operator - () const;
		Vec2<T> operator * (T k) const;
		Vec2<T> operator / (T k) const;
		Vec2<T>& operator *= (T k);
		Vec2<T>& operator /= (T k);

		T& operator [] (int i);
		const T& operator [] (int i) const;


		//-------------------------------------------------------------------------------

		static const Vec2<T> ZERO;
		static const Vec2<T> ONE;
		static const Vec2<T> UNIT_X;
		static const Vec2<T> UNIT_Y;

		//-------------------------------------------------------------------------------
	};

	//-------------------------------------------------------------------------------
	typedef Vec2<float> Vec2f;
	typedef Vec2<double> Vec2d;

	template<typename T> const Vec2<T> Vec2<T>::ZERO(0, 0);
	template<typename T> const Vec2<T> Vec2<T>::ONE(1, 1);
	template<typename T> const Vec2<T> Vec2<T>::UNIT_X(1, 0);
	template<typename T> const Vec2<T> Vec2<T>::UNIT_Y(0, 1);

	//-------------------------------------------------------------------------------

	template<typename T>
	Vec2<T>::Vec2() : x(0), y(0)
	{
	}
	template<typename T>
	Vec2<T>::Vec2(T vx, T vy) : x(vx), y(vy)
	{
	}
	template<typename T>
	Vec2<T>::Vec2(T v) : x(v), y(v)
	{
	}

	template<typename T>
	void Vec2<T>::Normalize()
	{
		T sum = x*x + y*y;
		if (sum > FLT_EPSILON)
		{
			T inv_len = math::InvSqrt(sum);
			x *= inv_len; y *= inv_len;
		}
	}
	template<typename T>
	Vec2<T> Vec2<T>::GetNormalized() const
	{
		T sum = x*x + y*y;
		if (sum > FLT_EPSILON)
		{
			T inv_len = math::InvSqrt(sum);
			return *this * inv_len;
		}
		return *this;
	}

	template<typename T>
	T Vec2<T>::Length() const
	{
		return math::Sqrt(x*x + y*y);
	}

	template<typename T>
	T Vec2<T>::Dot(const Vec2<T>& vec) const
	{
		return (x * vec.x + y * vec.y);
	}

	//-------------------------------------------------------------------------------
	// Operators
	//-------------------------------------------------------------------------------

	template<typename T>
	Vec2<T> Vec2<T>::operator - () const
	{
		return Vec2<T>(-x, -y);
	}

	template<typename T>
	Vec2<T> Vec2<T>::operator * (T k) const
	{
		return Vec2<T>(x*k, y*k);
	}
	template<typename T>
	Vec2<T> Vec2<T>::operator / (T k) const
	{
		k = (T)1.0 / k; return Vec2<T>(x*k, y*k);
	}
	template<typename T>
	Vec2<T>& Vec2<T>::operator *= (T k)
	{
		x *= k; y *= k; return *this;
	}
	template<typename T>
	Vec2<T>& Vec2<T>::operator /= (T k)
	{
		k = (T)1.0 / k; x *= k; y *= k; return *this;
	}

	template<typename T>
	T& Vec2<T>::operator [] (int i)
	{
		Assert(i >= 0 && i < 2);
		return ((T*)this)[i];
	}
	template<typename T>
	const T& Vec2<T>::operator [] (int i) const
	{
		Assert(i >= 0 && i < 2);
		return ((T*)this)[i];
	}

	//-------------------------------------------------------------------------------

	template<typename T>
	Vec2<T> operator * (T f, const Vec2<T>& vec)
	{
		return Vec2<T>((T)(f*vec.x), (T)(f*vec.y));
	}
	template<typename T1, typename T2>
	Vec2<T1> operator + (const Vec2<T1>& v0, const Vec2<T2>& v1)
	{
		return Vec2<T1>(v0.x + v1.x, v0.y + v1.y);
	}
	template<typename T1, typename T2>
	Vec2<T1> operator - (const Vec2<T1>& v0, const Vec2<T2>& v1)
	{
		return Vec2<T1>(v0.x - v1.x, v0.y - v1.y);
	}

	template<typename T1, typename T2>
	Vec2<T1>& operator += (Vec2<T1>& lhs, const Vec2<T2>& rhs)
	{
		lhs.x += rhs.x; lhs.y += rhs.y;
		return lhs;
	}
	template<typename T1, typename T2>
	Vec2<T1>& operator -= (Vec2<T1>& lhs, const Vec2<T2>& rhs)
	{
		lhs.x -= rhs.x; lhs.y -= rhs.y;
		return lhs;
	}

	//-------------------------------------------------------------------------------

} // namespace sb


#endif // _MATH_VEC2_H
