// Copyright 2008-2014 Simon Ekström

#ifndef _MATH_MATRIX4X4_H
#define _MATH_MATRIX4X4_H

#include "Math.h"


namespace sb
{

	template<typename T> class Vec3;
	template<typename T> class Vec4;
	template<typename T> class Matrix3x3;

	template<typename T> class Matrix4x4
	{
	public:
		T m00, m01, m02, m03;
		T m10, m11, m12, m13;
		T m20, m21, m22, m23;
		T m30, m31, m32, m33;


		//-------------------------------------------------------------------------------
		// Constructors 
		//-------------------------------------------------------------------------------

		Matrix4x4();
		explicit Matrix4x4(T x00, T x01, T x02, T x03,
			T x10, T x11, T x12, T x13,
			T x20, T x21, T x22, T x23,
			T x30, T x31, T x32, T x33);

		explicit Matrix4x4(const Matrix3x3<T>& m);


		//-------------------------------------------------------------------------------

		/// @brief Makes this matrix an identity matrix.
		void Identity();

		/// @brief Makes this matrix a zero matrix.
		void Zero();

		/// @brief Transposes this matrix.
		void Transpose();

		/// @brief Returns a transposed copy of this matrix.
		Matrix4x4<T> GetTransposed() const;

		/// @brief Returns true if the matrix is an identity matrix.
		bool IsIdentity() const;

		/// @brief Returns true if the matrix is a zero matrix.
		bool IsZero() const;

		/// @brief Inverts the matrix
		void Invert();

		/// @brief Returns an inverted copy of this matrix 
		Matrix4x4<T> GetInverted() const;

		/// @brief Returns this matrix determinant 
		float Determinant() const;

		/// @brief Creates a scale-matrix 
		void SetScale(const Vec3<T>& s);

		/// @brief Returns the translation part of this matrix.
		Vec3<T> GetTranslation() const;

		/// @brief Sets the translation of the matrix.
		void SetTranslation(const Vec3<T>& v);

		/// @brief Sets the specified row to the specified value.
		void SetRow(int row, const Vec4<T>& v);

		//-------------------------------------------------------------------------------
		// Operators
		//-------------------------------------------------------------------------------

		const T& operator () (uint32_t i, uint32_t j) const;
		T& operator () (uint32_t i, uint32_t j);

		Matrix4x4& operator=(const Matrix4x4& other);

		//-------------------------------------------------------------------------------

		static Matrix4x4<T> CreateIdentity();

		//-------------------------------------------------------------------------------
	};


	//-------------------------------------------------------------------------------

	typedef Matrix4x4<float> Mat4x4f;
	typedef Matrix4x4<double> Mat4x4d;

	//-------------------------------------------------------------------------------

	template<typename T>
	Matrix4x4<T>::Matrix4x4()
	{
		Zero();
	}
	template<typename T>
	Matrix4x4<T>::Matrix4x4(T x00, T x01, T x02, T x03,
		T x10, T x11, T x12, T x13,
		T x20, T x21, T x22, T x23,
		T x30, T x31, T x32, T x33)
	{
		m00 = x00; m01 = x01; m02 = x02; m03 = x03;
		m10 = x10; m11 = x11; m12 = x12; m13 = x13;
		m20 = x20; m21 = x21; m22 = x22; m23 = x23;
		m30 = x30; m31 = x31; m32 = x32; m33 = x33;
	}
	template<typename T>
	Matrix4x4<T>::Matrix4x4(const Matrix3x3<T>& m)
	{
		m00 = m.m00;	m01 = m.m01;	m02 = m.m02;	m03 = (T)0;
		m10 = m.m10;	m11 = m.m11;	m12 = m.m12;	m13 = (T)0;
		m20 = m.m20;	m21 = m.m21;	m22 = m.m22;	m23 = (T)0;
		m30 = (T)0;		m31 = (T)0;		m32 = (T)0;		m33 = (T)1;
	}

	//-------------------------------------------------------------------------------

	template<typename T>
	void Matrix4x4<T>::Identity()
	{
		m00 = 1; m01 = 0; m02 = 0; m03 = 0;
		m10 = 0; m11 = 1; m12 = 0; m13 = 0;
		m20 = 0; m21 = 0; m22 = 1; m23 = 0;
		m30 = 0; m31 = 0; m32 = 0; m33 = 1;
	}
	template<typename T>
	void Matrix4x4<T>::Zero()
	{
		m00 = 0; m01 = 0; m02 = 0; m03 = 0;
		m10 = 0; m11 = 0; m12 = 0; m13 = 0;
		m20 = 0; m21 = 0; m22 = 0; m23 = 0;
		m30 = 0; m31 = 0; m32 = 0; m33 = 0;
	}

	template<typename T>
	void Matrix4x4<T>::Transpose()
	{
		Matrix4x4<T> a = *this;

		m00 = a.m00; m01 = a.m10; m02 = a.m20; m03 = a.m30;
		m10 = a.m01; m11 = a.m11; m12 = a.m21; m13 = a.m31;
		m20 = a.m02; m21 = a.m12; m22 = a.m22; m23 = a.m32;
		m30 = a.m03; m31 = a.m13; m32 = a.m23; m33 = a.m33;
	}
	template<typename T>
	Matrix4x4<T> Matrix4x4<T>::GetTransposed() const
	{
		Matrix4x4<T> res;
		res.m00 = m00; res.m01 = m10; res.m02 = m20; res.m03 = m30;
		res.m10 = m01; res.m11 = m11; res.m12 = m21; res.m13 = m31;
		res.m20 = m02; res.m21 = m12; res.m22 = m22; res.m23 = m32;
		res.m30 = m03; res.m31 = m13; res.m32 = m23; res.m33 = m33;
		return res;
	}

	template<typename T>
	Matrix4x4<T> Matrix4x4<T>::GetInverted() const
	{
		Matrix4x4<T> res = *this;
		res.Invert();

		return res;
	}

	template<typename T>
	void Matrix4x4<T>::Invert()
	{
		Matrix4x4<T> m = *this;
		T det, inv_det;

		T m22m33 = m.m22 * m.m33;
		T m32m23 = m.m32 * m.m23;
		T m12m33 = m.m12 * m.m33;
		T m32m13 = m.m32 * m.m13;
		T m12m23 = m.m12 * m.m23;
		T m22m13 = m.m22 * m.m13;
		T m02m33 = m.m02 * m.m33;
		T m32m03 = m.m32 * m.m03;
		T m02m23 = m.m02 * m.m23;
		T m22m03 = m.m22 * m.m03;
		T m02m13 = m.m02 * m.m13;
		T m12m03 = m.m12 * m.m03;

		m00 = m22m33*m.m11 + m32m13*m.m21 + m12m23*m.m31;
		m00 -= m32m23*m.m11 + m12m33*m.m21 + m22m13*m.m31;
		m01 = m32m23*m.m01 + m02m33*m.m21 + m22m03*m.m31;
		m01 -= m22m33*m.m01 + m32m03*m.m21 + m02m23*m.m31;
		m02 = m12m33*m.m01 + m32m03*m.m11 + m02m13*m.m31;
		m02 -= m32m13*m.m01 + m02m33*m.m11 + m12m03*m.m31;
		m03 = m22m13*m.m01 + m02m23*m.m11 + m12m03*m.m21;
		m03 -= m12m23*m.m01 + m22m03*m.m11 + m02m13*m.m21;
		m10 = m32m23*m.m10 + m12m33*m.m20 + m22m13*m.m30;
		m10 -= m22m33*m.m10 + m32m13*m.m20 + m12m23*m.m30;
		m11 = m22m33*m.m00 + m32m03*m.m20 + m02m23*m.m30;
		m11 -= m32m23*m.m00 + m02m33*m.m20 + m22m03*m.m30;
		m12 = m32m13*m.m00 + m02m33*m.m10 + m12m03*m.m30;
		m12 -= m12m33*m.m00 + m32m03*m.m10 + m02m13*m.m30;
		m13 = m12m23*m.m00 + m22m03*m.m10 + m02m13*m.m20;
		m13 -= m22m13*m.m00 + m02m23*m.m10 + m12m03*m.m20;


		T m20m31 = m.m20*m.m31;
		T m30m21 = m.m30*m.m21;
		T m10m31 = m.m10*m.m31;
		T m30m11 = m.m30*m.m11;
		T m10m21 = m.m10*m.m21;
		T m20m11 = m.m20*m.m11;
		T m00m31 = m.m00*m.m31;
		T m30m01 = m.m30*m.m01;
		T m00m21 = m.m00*m.m21;
		T m20m01 = m.m20*m.m01;
		T m00m11 = m.m00*m.m11;
		T m10m01 = m.m10*m.m01;

		m20 = m20m31*m.m13 + m30m11*m.m23 + m10m21*m.m33;
		m20 -= m30m21*m.m13 + m10m31*m.m23 + m20m11*m.m33;
		m21 = m30m21*m.m03 + m00m31*m.m23 + m20m01*m.m33;
		m21 -= m20m31*m.m03 + m30m01*m.m23 + m00m21*m.m33;
		m22 = m10m31*m.m03 + m30m01*m.m13 + m00m11*m.m33;
		m22 -= m30m11*m.m03 + m00m31*m.m13 + m10m01*m.m33;
		m23 = m20m11*m.m03 + m00m21*m.m13 + m10m01*m.m23;
		m23 -= m10m21*m.m03 + m20m01*m.m13 + m00m11*m.m23;
		m30 = m10m31*m.m22 + m20m11*m.m32 + m30m21*m.m12;
		m30 -= m10m21*m.m32 + m20m31*m.m12 + m30m11*m.m22;
		m31 = m00m21*m.m32 + m20m31*m.m02 + m30m01*m.m22;
		m31 -= m00m31*m.m22 + m20m01*m.m32 + m30m21*m.m02;
		m32 = m00m31*m.m12 + m10m01*m.m32 + m30m11*m.m02;
		m32 -= m00m11*m.m32 + m10m31*m.m02 + m30m01*m.m12;
		m33 = m00m11*m.m22 + m10m21*m.m02 + m20m01*m.m12;
		m33 -= m00m21*m.m12 + m10m01*m.m22 + m20m11*m.m02;

		// Calculate determinant
		det = (m.m00*m00 + m.m10*m01 + m.m20*m02 + m.m30*m03);
#ifdef SANDBOX_BUILD_DEBUG
		//Assert(fabs(det) > 1e-14);
#endif

		inv_det = (T)1.0 / det;
		m00 *= inv_det; m01 *= inv_det; m02 *= inv_det; m03 *= inv_det;
		m10 *= inv_det; m11 *= inv_det; m12 *= inv_det; m13 *= inv_det;
		m20 *= inv_det; m21 *= inv_det; m22 *= inv_det; m23 *= inv_det;
		m30 *= inv_det; m31 *= inv_det; m32 *= inv_det; m33 *= inv_det;

	}
	//-------------------------------------------------------------------------------
	template<typename T>
	bool Matrix4x4<T>::IsIdentity() const
	{
		return (math::Fabs((T)1 - m00) + math::Fabs(m01) + math::Fabs(m02) + math::Fabs(m03) +
			math::Fabs(m10) + math::Fabs((T)1 - m11) + math::Fabs(m12) + math::Fabs(m13) +
			math::Fabs(m20) + math::Fabs(m21) + math::Fabs((T)1 - m22) + math::Fabs(m23) +
			math::Fabs(m30) + math::Fabs(m31) + math::Fabs(m32) + math::Fabs((T)1 - m33)) == 0;
	}
	template<typename T>
	bool Matrix4x4<T>::IsZero() const
	{
		return (math::Fabs(m00) + math::Fabs(m01) + math::Fabs(m02) + math::Fabs(m03) +
			math::Fabs(m10) + math::Fabs(m11) + math::Fabs(m12) + math::Fabs(m13) +
			math::Fabs(m20) + math::Fabs(m21) + math::Fabs(m22) + math::Fabs(m23) +
			math::Fabs(m30) + math::Fabs(m31) + math::Fabs(m32) + math::Fabs(m33)) == 0;
	}
	//-------------------------------------------------------------------------------
	template<typename T>
	float Matrix4x4<T>::Determinant() const
	{
		/*
		*	| m00 m01 m02 m03 |			| m11 m12 m13 |			| m01 m02 m03 |			| m01 m02 m03 |			| m01 m02 m03 |
		*	| m10 m11 m12 m13 |	= m00 *	| m21 m22 m23 | - m10 *	| m21 m22 m23 | + m20 *	| m11 m12 m13 | - m30 *	| m11 m12 m13 |
		*	| m20 m21 m22 m23 |			| m31 m32 m33 |			| m31 m32 m33 |			| m31 m32 m33 |			| m21 m22 m23 |
		*	| m30 m31 m32 m33 |
		*
		*/

		T det_00 = (m11*m22*m33) + (m12*m23*m31) + (m13*m21*m32) - (m13*m22*m31) - (m11*m23*m32) - (m12*m21*m33);
		T det_10 = (m01*m22*m33) + (m02*m23*m31) + (m03*m21*m32) - (m03*m22*m31) - (m01*m23*m32) - (m02*m21*m33);
		T det_20 = (m01*m12*m33) + (m02*m13*m31) + (m03*m11*m32) - (m03*m12*m31) - (m01*m13*m32) - (m02*m11*m33);
		T det_30 = (m01*m12*m23) + (m02*m13*m21) + (m03*m11*m22) - (m03*m12*m21) - (m01*m13*m22) - (m02*m11*m23);

		return (m00 * det_00 - m10 * det_10 + m20 * det_20 - m30 * det_30);
	}

	//-------------------------------------------------------------------------------
	template<typename T>
	void Matrix4x4<T>::SetScale(const Vec3<T>& s)
	{
		m00 = s.x;	m01 = 0;	m02 = 0;	m03 = 0;
		m10 = 0;	m11 = s.y;	m12 = 0;	m13 = 0;
		m20 = 0;	m21 = 0;	m22 = s.z;	m23 = 0;
		m30 = 0;	m31 = 0;	m32 = 0;	m33 = T(1);
	}

	//-------------------------------------------------------------------------------

	template<typename T>
	Vec3<T> Matrix4x4<T>::GetTranslation() const
	{
		return Vec3<T>(m03, m13, m23);
	}
	template<typename T>
	void Matrix4x4<T>::SetTranslation(const Vec3<T>& v)
	{
		m03 = v.x;
		m13 = v.y;
		m23 = v.z;
	}
	//-------------------------------------------------------------------------------
	template<typename T>
	void Matrix4x4<T>::SetRow(int row, const Vec4<T>& v)
	{
		Assert(row < 4);
		T* ptr = (T*)&m00;

		ptr[0 + 4 * row] = v.x;
		ptr[1 + 4 * row] = v.y;
		ptr[2 + 4 * row] = v.z;
		ptr[3 + 4 * row] = v.w;
	}

	//-------------------------------------------------------------------------------
	// Operators
	//-------------------------------------------------------------------------------

	template<typename T>
	const T& Matrix4x4<T>::operator () (uint32_t i, uint32_t j) const
	{
		Assert((i < 4) && (j < 4));
		T* p = (T*)&m00;
		return p[i * 4 + j];
	}
	template<typename T>
	T& Matrix4x4<T>::operator () (uint32_t i, uint32_t j)
	{
		Assert((i < 4) && (j < 4));
		T* p = (T*)&m00;
		return p[i * 4 + j];
	}
	template<typename T>
	Matrix4x4<T>& Matrix4x4<T>::operator=(const Matrix4x4<T>& other)
	{
		m00 = other.m00;	m01 = other.m01;	m02 = other.m02;	m03 = other.m03;
		m10 = other.m10;	m11 = other.m11;	m12 = other.m12;	m13 = other.m13;
		m20 = other.m20;	m21 = other.m21;	m22 = other.m22;	m23 = other.m23;
		m30 = other.m30;	m31 = other.m31;	m32 = other.m32;	m33 = other.m33;

		return *this;
	}

	//-------------------------------------------------------------------------------

	template<typename T>
	Matrix4x4<T> Matrix4x4<T>::CreateIdentity()
	{
		Matrix4x4<T> m;
		m.Identity();
		return m;
	}
	//-------------------------------------------------------------------------------

	template<typename T1, typename T2>
	Matrix4x4<T1> operator* (const Matrix4x4<T1> &lhs, const Matrix4x4<T2> &rhs)
	{
		Matrix4x4<T1> result;

		// Matrix multiplication [Real-Time Rendering, A.26, page 899]

		result.m00 = rhs.m00 * lhs.m00 + rhs.m10 * lhs.m01 + rhs.m20 * lhs.m02 + rhs.m30 * lhs.m03;
		result.m10 = rhs.m00 * lhs.m10 + rhs.m10 * lhs.m11 + rhs.m20 * lhs.m12 + rhs.m30 * lhs.m13;
		result.m20 = rhs.m00 * lhs.m20 + rhs.m10 * lhs.m21 + rhs.m20 * lhs.m22 + rhs.m30 * lhs.m23;
		result.m30 = rhs.m00 * lhs.m30 + rhs.m10 * lhs.m31 + rhs.m20 * lhs.m32 + rhs.m30 * lhs.m33;

		result.m01 = rhs.m01 * lhs.m00 + rhs.m11 * lhs.m01 + rhs.m21 * lhs.m02 + rhs.m31 * lhs.m03;
		result.m11 = rhs.m01 * lhs.m10 + rhs.m11 * lhs.m11 + rhs.m21 * lhs.m12 + rhs.m31 * lhs.m13;
		result.m21 = rhs.m01 * lhs.m20 + rhs.m11 * lhs.m21 + rhs.m21 * lhs.m22 + rhs.m31 * lhs.m23;
		result.m31 = rhs.m01 * lhs.m30 + rhs.m11 * lhs.m31 + rhs.m21 * lhs.m32 + rhs.m31 * lhs.m33;

		result.m02 = rhs.m02 * lhs.m00 + rhs.m12 * lhs.m01 + rhs.m22 * lhs.m02 + rhs.m32 * lhs.m03;
		result.m12 = rhs.m02 * lhs.m10 + rhs.m12 * lhs.m11 + rhs.m22 * lhs.m12 + rhs.m32 * lhs.m13;
		result.m22 = rhs.m02 * lhs.m20 + rhs.m12 * lhs.m21 + rhs.m22 * lhs.m22 + rhs.m32 * lhs.m23;
		result.m32 = rhs.m02 * lhs.m30 + rhs.m12 * lhs.m31 + rhs.m22 * lhs.m32 + rhs.m32 * lhs.m33;

		result.m03 = rhs.m03 * lhs.m00 + rhs.m13 * lhs.m01 + rhs.m23 * lhs.m02 + rhs.m33 * lhs.m03;
		result.m13 = rhs.m03 * lhs.m10 + rhs.m13 * lhs.m11 + rhs.m23 * lhs.m12 + rhs.m33 * lhs.m13;
		result.m23 = rhs.m03 * lhs.m20 + rhs.m13 * lhs.m21 + rhs.m23 * lhs.m22 + rhs.m33 * lhs.m23;
		result.m33 = rhs.m03 * lhs.m30 + rhs.m13 * lhs.m31 + rhs.m23 * lhs.m32 + rhs.m33 * lhs.m33;

		return result;
	}

	template<typename T1, typename T2>
	Matrix4x4<T1> operator* (const Matrix4x4<T1> &lhs, const Matrix3x3<T2> &rhs)
	{
		Matrix4x4<T1> result;

		// Matrix multiplication [Real-Time Rendering, A.26, page 899]

		result.m00 = rhs.m00 * lhs.m00 + rhs.m10 * lhs.m01 + rhs.m20 * lhs.m02;
		result.m10 = rhs.m00 * lhs.m10 + rhs.m10 * lhs.m11 + rhs.m20 * lhs.m12;
		result.m20 = rhs.m00 * lhs.m20 + rhs.m10 * lhs.m21 + rhs.m20 * lhs.m22;
		result.m30 = rhs.m00 * lhs.m30 + rhs.m10 * lhs.m31 + rhs.m20 * lhs.m32;

		result.m01 = rhs.m01 * lhs.m00 + rhs.m11 * lhs.m01 + rhs.m21 * lhs.m02;
		result.m11 = rhs.m01 * lhs.m10 + rhs.m11 * lhs.m11 + rhs.m21 * lhs.m12;
		result.m21 = rhs.m01 * lhs.m20 + rhs.m11 * lhs.m21 + rhs.m21 * lhs.m22;
		result.m31 = rhs.m01 * lhs.m30 + rhs.m11 * lhs.m31 + rhs.m21 * lhs.m32;

		result.m02 = rhs.m02 * lhs.m00 + rhs.m12 * lhs.m01 + rhs.m22 * lhs.m02;
		result.m12 = rhs.m02 * lhs.m10 + rhs.m12 * lhs.m11 + rhs.m22 * lhs.m12;
		result.m22 = rhs.m02 * lhs.m20 + rhs.m12 * lhs.m21 + rhs.m22 * lhs.m22;
		result.m32 = rhs.m02 * lhs.m30 + rhs.m12 * lhs.m31 + rhs.m22 * lhs.m32;

		result.m03 = lhs.m03;
		result.m13 = lhs.m13;
		result.m23 = lhs.m23;
		result.m33 = lhs.m33;

		return result;
	}


	template<typename T>
	Matrix4x4<T> operator* (const Matrix4x4<T> &m, T a)
	{
		Matrix4x4<T> result;
		result.m00 = m.m00 * a;	result.m01 = m.m01 * a;	result.m02 = m.m02 * a;	result.m03 = m.m03 * a;
		result.m10 = m.m10 * a;	result.m11 = m.m11 * a;	result.m12 = m.m12 * a;	result.m13 = m.m13 * a;
		result.m20 = m.m20 * a;	result.m21 = m.m21 * a;	result.m22 = m.m22 * a;	result.m23 = m.m23 * a;
		result.m30 = m.m30 * a;	result.m31 = m.m31 * a;	result.m32 = m.m32 * a;	result.m33 = m.m33 * a;

		return result;
	}

	template<typename T>
	Matrix4x4<T> operator/ (const Matrix4x4<T> &m, T a)
	{
		return m * ((T)1.0 / a);
	}


	template<typename T1, typename T2>
	Vec4<T1> operator* (const Matrix4x4<T2>& m, const Vec4<T1>& v)
	{
		return Vec4<T1>(v.x * m.m00 + v.y * m.m01 + v.z * m.m02 + v.w * m.m03,
			v.x * m.m10 + v.y * m.m11 + v.z * m.m12 + v.w * m.m13,
			v.x * m.m20 + v.y * m.m21 + v.z * m.m22 + v.w * m.m23,
			v.x * m.m30 + v.y * m.m31 + v.z * m.m32 + v.w * m.m33);
	}

	template<typename T1, typename T2>
	Vec4<T1> operator* (const Vec4<T1>& v, const Matrix4x4<T2>& m)
	{
		return Vec4<T1>(v.x * m.m00 + v.y * m.m10 + v.z * m.m20 + v.w * m.m30,
			v.x * m.m01 + v.y * m.m11 + v.z * m.m21 + v.w * m.m31,
			v.x * m.m02 + v.y * m.m12 + v.z * m.m22 + v.w * m.m32,
			v.x * m.m03 + v.y * m.m13 + v.z * m.m23 + v.w * m.m33);
	}
	template<typename T1, typename T2>
	Vec3<T1> operator* (const Matrix4x4<T2>& m, const Vec3<T1>& v)
	{
		return Vec3<T1>(v.x * m.m00 + v.y * m.m01 + v.z * m.m02 + m.m03,
			v.x * m.m10 + v.y * m.m11 + v.z * m.m12 + m.m13,
			v.x * m.m20 + v.y * m.m21 + v.z * m.m22 + m.m23);
	}

	template<typename T1, typename T2>
	Vec3<T1> operator* (const Vec3<T1>& v, const Matrix4x4<T2>& m)
	{
		return Vec3<T1>(v.x * m.m00 + v.y * m.m10 + v.z * m.m20 + m.m30,
			v.x * m.m01 + v.y * m.m11 + v.z * m.m21 + m.m31,
			v.x * m.m02 + v.y * m.m12 + v.z * m.m22 + m.m32);
	}


	//-------------------------------------------------------------------------------

} // namespace sb





#endif // _MATH_MATRIX4X4_H
