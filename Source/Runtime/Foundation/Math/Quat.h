// Copyright 2008-2014 Simon Ekström

#ifndef _MATH_QUAT_H
#define _MATH_QUAT_H


namespace sb
{

	template<typename T> class Vec3;
	template<typename T>
	class Quat
	{
	public:
		T x, y, z, w;

		Quat();
		Quat(T tx, T ty, T tz, T tw);

		//-------------------------------------------------------------------------------
		void Set(T tx, T ty, T tz, T tw);

		/// @brief Returns the length of the quaternion
		T Length() const;

		/// @brief Normalizes the Quaternion
		void Normalize();

		/// @brief Returns a normalized copy
		Quat<T> GetNormalized() const;


		/// @brief Inverts the quaternion
		void Invert();

		/// @brief Returns an inverted copy of this quaternion
		Quat<T> GetInverted() const;

		void SetIdentity();


		void SetRotation(T angle, const Vec3<T>& axis);
		void SetRotationX(const float rad);
		void SetRotationY(const float rad);
		void SetRotationZ(const float rad);

		//-------------------------------------------------------------------------------
		// Operators

		INLINE Quat<T> operator-() const;

		//-------------------------------------------------------------------------------
		INLINE static Quat<T> CreateRotation(T angle, const Vec3<T>& axis);

		//-------------------------------------------------------------------------------
	};

	//-------------------------------------------------------------------------------
	typedef Quat<float> Quatf;

	//-------------------------------------------------------------------------------
	template<typename T>
	Quat<T>::Quat() : x(0), y(0), z(0), w(0)
	{
	}

	template<typename T>
	Quat<T>::Quat(T tx, T ty, T tz, T tw)
	{
		x = tx; y = ty; z = tz; w = tw;
	}

	//-------------------------------------------------------------------------------
	template<typename T>
	void Quat<T>::Set(T tx, T ty, T tz, T tw)
	{
		x = tx; y = ty; z = tz; w = tw;
	}

	template<typename T>
	T Quat<T>::Length() const
	{
		return math::Sqrt(x*x + y*y + z*z + w*w);
	}

	template<typename T>
	void Quat<T>::Normalize()
	{
		T d = math::InvSqrt(x*x + y*y + z*z + w*w);
		x *= d; y *= d; z *= d; w *= d;
	}
	template<typename T>
	Quat<T> Quat<T>::GetNormalized() const
	{
		Quat<T> r = *this;
		r.Normalize();
		return r;
	}

	template<typename T>
	void Quat<T>::Invert()
	{
		x = -x; y = -y; z = -z;
	}

	template<typename T>
	Quat<T> Quat<T>::GetInverted() const
	{
		Quat<T> res = *this;
		res.Invert();
		return res;
	}

	template<typename T>
	void Quat<T>::SetIdentity()
	{
		x = 0; y = 0; z = 0; w = 1;
	}

	template<typename T>
	void Quat<T>::SetRotation(T angle, const Vec3<T>& axis)
	{
		float s, c; math::SinCos(angle*(T)0.5, s, c);
		w = c;
		x = axis.x * s;
		y = axis.y * s;
		z = axis.z * s;
	}
	template<typename T>
	void Quat<T>::SetRotationX(const float rad)
	{
		float s, c; math::SinCos(angle*(T)0.5, s, c);
		w = c; x = s; y = 0; z = 0;
	}
	template<typename T>
	void Quat<T>::SetRotationY(const float rad)
	{
		float s, c; math::SinCos(angle*(T)0.5, s, c);
		w = c; x = 0; y = s; z = 0;
	}
	template<typename T>
	void Quat<T>::SetRotationZ(const float rad)
	{
		float s, c; math::SinCos(angle*(T)0.5, s, c);
		w = c; x = 0; y = 0; z = s;
	}

	//-------------------------------------------------------------------------------

	template<typename T>
	Quat<T> operator-() const
	{
		return Quat<T>(-x, -y, -z, -w);
	}

	//-------------------------------------------------------------------------------
	template<typename T>
	INLINE Quat<T> Quat<T>::CreateRotation(T angle, const Vec3<T>& axis)
	{
		Quat<T> q; q.SetRotation(angle, axis); return q;
	}

	//-------------------------------------------------------------------------------

} // namespace sb




#endif // _MATH_QUAT_H