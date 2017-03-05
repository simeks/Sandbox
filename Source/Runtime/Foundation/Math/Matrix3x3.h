// Copyright 2008-2014 Simon Ekström

#ifndef _MATH_MATRIX3X3_H
#define _MATH_MATRIX3X3_H

#include "Math.h"


namespace sb
{

	template<typename T> class Quat;
	template<typename T> class Vec2;
	template<typename T> class Vec3;


	template<typename T> class Matrix3x3
	{
	public:
		T m00, m01, m02;
		T m10, m11, m12;
		T m20, m21, m22;


		//-------------------------------------------------------------------------------
		// Constructors 
		//-------------------------------------------------------------------------------

		Matrix3x3();
		explicit Matrix3x3(T x00, T x01, T x02,
			T x10, T x11, T x12,
			T x20, T x21, T x22);
		explicit Matrix3x3(const Quat<T> &q);

		//-------------------------------------------------------------------------------

		void Identity();
		void Zero();

		void Transpose();
		Matrix3x3<T> GetTransposed() const;

		void Invert();

		bool IsIdentity() const;
		bool IsZero() const;

		/// @brief Returns an inverted copy of this matrix 
		Matrix3x3<T> GetInverted() const;

		/// @brief Returns this matrix determinant 
		float Determinant() const;

		/// @brief Creates a scale-matrix 
		void SetScale(const Vec3<T>& s);

		/// @brief Creates a rotation-matrix around the given axis vector 
		///	@param axis Axis vector, should be normalized
		void SetRotation(T angle, const Vec3<T>& axis);

		/// @brief Creates a rotation-matrix
		void SetRotation(const Vec3<T>& rot);

		/// @brief Creates a rotation-matrix around the x-axis using the given angle
		///	@param rad Angle in radians
		void SetRotationX(const float rad);

		/// @brief Creates a rotation-matrix around the x-axis using the given angle
		///	@param rad Angle in radians
		void SetRotationY(const float rad);

		/// @brief Creates a rotation-matrix around the x-axis using the given angle
		///	@param rad Angle in radians
		void SetRotationZ(const float rad);

		//-------------------------------------------------------------------------------
		// Operators
		//-------------------------------------------------------------------------------

		const T& operator () (uint32_t i, uint32_t j) const;
		T& operator () (uint32_t i, uint32_t j);

		Matrix3x3<T> operator - ();
		Matrix3x3<T>& operator *= (const T a);
		Matrix3x3<T>& operator /= (const T a);

		Matrix3x3& operator=(const Matrix3x3& other);

		//-------------------------------------------------------------------------------

		INLINE static Matrix3x3<T> CreateIdentity();

		INLINE static Matrix3x3<T> CreateScale(const Vec3<T>& s);

		INLINE static Matrix3x3<T> CreateRotation(T angle, const Vec3<T>& axis);
		INLINE static Matrix3x3<T> CreateRotation(const Vec3<T>& rot);
		INLINE static Matrix3x3<T> CreateRotationX(const float rad);
		INLINE static Matrix3x3<T> CreateRotationY(const float rad);
		INLINE static Matrix3x3<T> CreateRotationZ(const float rad);

		INLINE static Matrix3x3<T> CreateOrientation(const Vec3<T>& forward, const Vec3<T>& up);

		//-------------------------------------------------------------------------------

	};

	//-------------------------------------------------------------------------------
	typedef Matrix3x3<float> Mat3x3f;
	typedef Matrix3x3<double> Mat3x3d;


	//-------------------------------------------------------------------------------
	template<typename T>
	Matrix3x3<T>::Matrix3x3()
	{
		Zero();
	}
	template<typename T>
	Matrix3x3<T>::Matrix3x3(T x00, T x01, T x02,
		T x10, T x11, T x12,
		T x20, T x21, T x22)
	{
		m00 = x00; m01 = x01; m02 = x02;
		m10 = x10; m11 = x11; m12 = x12;
		m20 = x20; m21 = x21; m22 = x22;
	}
	template<typename T>
	Matrix3x3<T>::Matrix3x3(const Quat<T> &q)
	{
		float xx, xy, xz;
		float yy, yz, zz;
		float wx, wy, wz;

		float x2, y2, z2;
		x2 = q.x + q.x;
		y2 = q.y + q.y;
		z2 = q.z + q.z;

		xx = q.x * x2;	xy = q.x * y2;	xz = q.x * z2;
		yy = q.y * y2;	yz = q.y * z2;	zz = q.z * z2;
		wx = q.w * x2;	wy = q.w * y2;	wz = q.w * z2;

		m00 = 1 - (yy + zz);	m01 = xy - wz;			m02 = xz + wy;
		m10 = xy + wz;			m11 = 1 - (xx + zz);	m12 = yz - wx;
		m20 = xz - wy;			m21 = yz - wx;			m22 = 1 - (xx + yy);

	}

	//-------------------------------------------------------------------------------

	template<typename T>
	void Matrix3x3<T>::Identity()
	{
		m00 = 1; m01 = 0; m02 = 0;
		m10 = 0; m11 = 1; m12 = 0;
		m20 = 0; m21 = 0; m22 = 1;
	}
	template<typename T>
	void Matrix3x3<T>::Zero()
	{
		m00 = 0; m01 = 0; m02 = 0;
		m10 = 0; m11 = 0; m12 = 0;
		m20 = 0; m21 = 0; m22 = 0;
	}

	template<typename T>
	void Matrix3x3<T>::Transpose()
	{
		T t;
		t = m01; m01 = m10; m10 = t;
		t = m02; m02 = m20; m20 = t;
		t = m12; m12 = m21; m21 = t;
	}

	template<typename T>
	Matrix3x3<T> Matrix3x3<T>::GetTransposed() const
	{
		Matrix3x3<T> res;
		res.m00 = m00; res.m01 = m10; res.m02 = m20;
		res.m10 = m01; res.m11 = m11; res.m12 = m21;
		res.m20 = m02; res.m21 = m12; res.m22 = m22;
		return res;
	}

	//-------------------------------------------------------------------------------
	template<typename T>
	void Matrix3x3<T>::Invert()
	{
		Matrix3x3<T> m = *this;
		T det, inv_det;

		m00 = m.m22*m.m11 - m.m12*m.m21;	m01 = m.m02*m.m21 - m.m22*m.m01;	m02 = m.m12*m.m01 - m.m02*m.m11;
		m10 = m.m12*m.m20 - m.m22*m.m10;	m11 = m.m22*m.m00 - m.m02*m.m20;	m12 = m.m02*m.m10 - m.m12*m.m00;
		m20 = m.m10*m.m21 - m.m20*m.m11;	m21 = m.m20*m.m01 - m.m00*m.m21;	m22 = m.m00*m.m11 - m.m10*m.m01;

		det = (m.m00*m00 + m.m10*m01 + m.m20*m02);
#ifdef SANDBOX_BUILD_DEBUG
		Assert(math::Fabs(det) > 1E-20f);
#endif

		inv_det = (T)1.0 / det;
		m00 *= inv_det;	m01 *= inv_det;	m02 *= inv_det;
		m10 *= inv_det;	m11 *= inv_det;	m12 *= inv_det;
		m20 *= inv_det;	m21 *= inv_det;	m22 *= inv_det;

	}
	//-------------------------------------------------------------------------------
	template<typename T>
	bool Matrix3x3<T>::IsIdentity() const
	{
		return (math::Fabs((T)1 - m00) + math::Fabs(m01) + math::Fabs(m02) +
			math::Fabs(m10) + math::Fabs((T)1 - m11) + math::Fabs(m12) +
			math::Fabs(m20) + math::Fabs(m21) + math::Fabs((T)1 - m22)) == 0;
	}
	template<typename T>
	bool Matrix3x3<T>::IsZero() const
	{
		return (math::Fabs(m00) + math::Fabs(m01) + math::Fabs(m02) +
			math::Fabs(m10) + math::Fabs(m11) + math::Fabs(m12) +
			math::Fabs(m20) + math::Fabs(m21) + math::Fabs(m22)) == 0;
	}
	//-------------------------------------------------------------------------------

	template<typename T>
	Matrix3x3<T> Matrix3x3<T>::GetInverted() const
	{
		Matrix3x3<T> res = *this;
		res.Invert();

		return res;
	}

	template<typename T>
	float Matrix3x3<T>::Determinant() const
	{
		return (m00*m11*m22) + (m01*m12*m20) + (m02*m10*m21) - (m02*m11*m20) - (m00*m12*m21) - (m01*m10*m22);
	}

	template<typename T>
	void Matrix3x3<T>::SetScale(const Vec3<T>& s)
	{
		m00 = s.x;	m01 = 0;	m02 = 0;
		m10 = 0;	m11 = s.y;	m12 = 0;
		m20 = 0;	m21 = 0;	m22 = s.z;
	}

	template<typename T>
	void Matrix3x3<T>::SetRotation(T angle, const Vec3<T>& axis)
	{
		double s, c; math::SinCos(angle, s, c);
		double mc = 1.0f - c;
		double mcx = mc*axis.x;	double mcy = mc*axis.y;	double mcz = mc*axis.z;
		double tcx = s*axis.x;	double tcy = s*axis.y;	double tcz = s*axis.z;

		m00 = (T)(mcx*axis.x + c);	m01 = (T)(mcx*axis.y - tcz);	m02 = (T)(mcx*axis.z + tcy);
		m10 = (T)(mcy*axis.x + tcz);	m11 = (T)(mcy*axis.y + c);	m12 = (T)(mcy*axis.z - tcx);
		m20 = (T)(mcz*axis.x - tcy);	m21 = (T)(mcz*axis.y + tcx);	m22 = (T)(mcz*axis.z + c);
	}
	template<typename T>
	void Matrix3x3<T>::SetRotation(const Vec3<T>& rot)
	{
		T a = rot.Length();
		if (a == T(0))
		{
			Identity();
		}
		else
		{
			SetRotation(a, rot / a);
		}
	}

	/// @brief Creates a rotation-matrix around the x-axis using the given angle
	///	@param rad Angle in radians
	template<typename T>
	void Matrix3x3<T>::SetRotationX(const float rad)
	{
		T s, c;	math::SinCos(rad, s, c);
		m00 = 1;	m01 = 0;	m02 = 0;
		m10 = 0;	m11 = c;	m12 = -s;
		m20 = 0;	m21 = s;	m22 = c;
	}
	/// @brief Creates a rotation-matrix around the x-axis using the given angle
	///	@param rad Angle in radians
	template<typename T>
	void Matrix3x3<T>::SetRotationY(const float rad)
	{
		T s, c;	math::SinCos(rad, s, c);
		m00 = c;	m01 = 0;	m02 = s;
		m10 = 0;	m11 = 1;	m12 = 0;
		m20 = -s;	m21 = 0;	m22 = c;
	}
	/// @brief Creates a rotation-matrix around the x-axis using the given angle
	///	@param rad Angle in radians
	template<typename T>
	void Matrix3x3<T>::SetRotationZ(const float rad)
	{
		T s, c;	math::SinCos(rad, s, c);
		m00 = c;	m01 = -s;	m02 = 0;
		m10 = s;	m11 = c;	m12 = 0;
		m20 = 0;	m21 = 0;	m22 = 1;
	}

	//-------------------------------------------------------------------------------
	// Operators
	//-------------------------------------------------------------------------------

	template<typename T>
	const T& Matrix3x3<T>::operator () (uint32_t i, uint32_t j) const
	{
		Assert((i < 3) && (j < 3));
		T* p = (T*)&m00;
		return p[i * 3 + j];
	}
	template<typename T>
	T& Matrix3x3<T>::operator () (uint32_t i, uint32_t j)
	{
		Assert((i < 3) && (j < 3));
		T* p = (T*)&m00;
		return p[i * 3 + j];
	}

	template<typename T>
	Matrix3x3<T> Matrix3x3<T>::operator - ()
	{
		return Matrix3x3(-m00, -m01, -m02, -m10, -m11, -m12, -m20, -m21, -m22);
	}

	template<typename T>
	Matrix3x3<T>& Matrix3x3<T>::operator *= (const T a)
	{
		m00 *= a;	m01 *= a;	m02 *= a;
		m10 *= a;	m11 *= a;	m12 *= a;
		m20 *= a;	m21 *= a;	m22 *= a;
		return *this;
	}
	template<typename T>
	Matrix3x3<T>& Matrix3x3<T>::operator /= (const T a)
	{
		T inv = (T)1.0 / a;
		m00 *= inv;	m01 *= inv;	m02 *= inv;
		m10 *= inv;	m11 *= inv;	m12 *= inv;
		m20 *= inv;	m21 *= inv;	m22 *= inv;
		return *this;
	}

	template<typename T>
	Matrix3x3<T>& Matrix3x3<T>::operator=(const Matrix3x3<T>& other)
	{
		m00 = other.m00;	m01 = other.m01;	m02 = other.m02;
		m10 = other.m10;	m11 = other.m11;	m12 = other.m12;
		m20 = other.m20;	m21 = other.m21;	m22 = other.m22;

		return *this;
	}


	//-------------------------------------------------------------------------------

	template<typename T>
	INLINE Matrix3x3<T> Matrix3x3<T>::CreateIdentity()
	{
		Matrix3x3<T> m;
		m.Identity();
		return m;
	}
	template<typename T>
	INLINE Matrix3x3<T> Matrix3x3<T>::CreateScale(const Vec3<T>& s)
	{
		Matrix3x3<T> m;
		m.SetScale(s);
		return m;
	}
	template<typename T>
	INLINE Matrix3x3<T> Matrix3x3<T>::CreateRotation(T angle, const Vec3<T>& axis)
	{
		Matrix3x3<T> m;
		m.SetRotation(angle, axis);
		return m;
	}
	template<typename T>
	INLINE Matrix3x3<T> Matrix3x3<T>::CreateRotation(const Vec3<T>& rot)
	{
		Matrix3x3<T> m;
		m.SetRotation(rot);
		return m;
	}
	template<typename T>
	INLINE Matrix3x3<T> Matrix3x3<T>::CreateRotationX(const float rad)
	{
		Matrix3x3<T> m;
		m.SetRotationX(rad);
		return m;
	}
	template<typename T>
	INLINE Matrix3x3<T> Matrix3x3<T>::CreateRotationY(const float rad)
	{
		Matrix3x3<T> m;
		m.SetRotationY(rad);
		return m;
	}
	template<typename T>
	INLINE Matrix3x3<T> Matrix3x3<T>::CreateRotationZ(const float rad)
	{
		Matrix3x3<T> m;
		m.SetRotationZ(rad);
		return m;
	}

	template<typename T>
	INLINE Matrix3x3<T> Matrix3x3<T>::CreateOrientation(const Vec3<T>& forward, const Vec3<T>& up)
	{
		Vec3<T> xaxis, yaxis, zaxis;
		zaxis = -forward.GetNormalized();
		xaxis = up.Cross(zaxis).GetNormalized();
		yaxis = zaxis.Cross(xaxis);

		Matrix3x3<T> m;
		m.m00 = xaxis.x;	m.m01 = yaxis.x;	m.m02 = zaxis.x;
		m.m10 = xaxis.y;	m.m11 = yaxis.y;	m.m12 = zaxis.y;
		m.m20 = xaxis.z;	m.m21 = yaxis.z;	m.m22 = zaxis.z;

		return m;
	}
	//-------------------------------------------------------------------------------

	template<typename T1, typename T2>
	Matrix3x3<T1> operator* (const Matrix3x3<T1> &lhs, const Matrix3x3<T2> &rhs)
	{
		Matrix3x3<T1> result;
		result.m00 = lhs.m00*rhs.m00 + lhs.m01*rhs.m10 + lhs.m02*rhs.m20;
		result.m01 = lhs.m00*rhs.m01 + lhs.m01*rhs.m11 + lhs.m02*rhs.m21;
		result.m02 = lhs.m00*rhs.m02 + lhs.m01*rhs.m12 + lhs.m02*rhs.m22;

		result.m10 = lhs.m10*rhs.m00 + lhs.m11*rhs.m10 + lhs.m12*rhs.m20;
		result.m11 = lhs.m10*rhs.m01 + lhs.m11*rhs.m11 + lhs.m12*rhs.m21;
		result.m12 = lhs.m10*rhs.m02 + lhs.m11*rhs.m12 + lhs.m12*rhs.m22;

		result.m20 = lhs.m20*rhs.m00 + lhs.m21*rhs.m10 + lhs.m22*rhs.m20;
		result.m21 = lhs.m20*rhs.m01 + lhs.m21*rhs.m11 + lhs.m22*rhs.m21;
		result.m22 = lhs.m20*rhs.m02 + lhs.m21*rhs.m12 + lhs.m22*rhs.m22;

		return result;
	}


	template<typename T>
	Matrix3x3<T> operator* (const Matrix3x3<T> &m, T a)
	{
		Matrix3x3<T> res;
		res.m00 = m.m00 * a;	res.m01 = m.m01 * a;	res.m02 = m.m02 * a;
		res.m10 = m.m10 * a;	res.m11 = m.m11 * a;	res.m12 = m.m12 * a;
		res.m20 = m.m20 * a;	res.m21 = m.m21 * a;	res.m22 = m.m22 * a;
		return res;
	}

	template<typename T>
	Matrix3x3<T> operator/ (const Matrix3x3<T> &m, T a)
	{
		return m * ((T)1.0 / a);
	}


	template<typename T1, typename T2>
	Vec3<T1> operator* (const Matrix3x3<T2>& m, const Vec3<T1>& v)
	{
		return Vec3<T1>(v.x * m.m00 + v.y * m.m01 + v.z * m.m02,
			v.x * m.m10 + v.y * m.m11 + v.z * m.m12,
			v.x * m.m20 + v.y * m.m21 + v.z * m.m22);
	}

	template<typename T1, typename T2>
	Vec3<T1> operator* (const Vec3<T1>& v, const Matrix3x3<T2>& m)
	{
		return Vec3<T1>(v.x * m.m00 + v.y * m.m10 + v.z * m.m20,
			v.x * m.m01 + v.y * m.m11 + v.z * m.m21,
			v.x * m.m02 + v.y * m.m12 + v.z * m.m22);
	}


	template<typename T1, typename T2>
	Vec2<T1> operator* (const Matrix3x3<T2>& m, const Vec2<T1>& v)
	{
		return Vec2<T1>(v.x * m.m00 + v.y * m.m01, v.x * m.m10 + v.y * m.m11);
	}

	template<typename T1, typename T2>
	Vec2<T1> operator* (const Vec2<T1>& v, const Matrix3x3<T2>& m)
	{
		return Vec2<T1>(v.x * m.m00 + v.y * m.m10, v.x * m.m01 + v.y * m.m11);
	}

	//-------------------------------------------------------------------------------

} // namespace sb





#endif // _MATH_MATRIX3X3_H