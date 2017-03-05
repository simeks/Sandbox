// Copyright 2008-2014 Simon Ekström

#ifndef _MATH_MATRIXUTIL_H
#define _MATH_MATRIXUTIL_H


namespace sb
{

	namespace matrix_util
	{
		/// @brief Creates a right-handed view matrix from the specified parameters.
		INLINE Mat4x4f CreateLookAt(const Vec3f& eye, const Vec3f& at, const Vec3f& up);

		/// @brief Creates a right-handed ortographic projection matrix.
		INLINE Mat4x4f CreateOrtho(float w, float h, float zn, float zf);

		/// @brief Creates a right-handed ortographic projection matrix.
		INLINE Mat4x4f CreateOrthoOffCenter(float l, float r, float b, float t, float zn, float zf);

		/// @brief Creates a right-handed perspective projection matrix.
		INLINE Mat4x4f CreatePerspectiveFov(float fovy, float aspect, float zn, float zf);

	};

	Mat4x4f matrix_util::CreateLookAt(const Vec3f& eye, const Vec3f& at, const Vec3f& up)
	{
		Mat4x4f m;

		Vec3f dir = eye - at;
		Vec3f zaxis = dir.GetNormalized();
		Vec3f xaxis = (up.Cross(zaxis)).GetNormalized();
		Vec3f yaxis = zaxis.Cross(xaxis);

		m.m00 = xaxis.x;	m.m01 = yaxis.x;	m.m02 = zaxis.x;	m.m03 = -xaxis.Dot(eye);
		m.m10 = xaxis.y;	m.m11 = yaxis.y;	m.m12 = zaxis.y;	m.m13 = -yaxis.Dot(eye);
		m.m20 = xaxis.z;	m.m21 = yaxis.z;	m.m22 = zaxis.z;	m.m23 = -zaxis.Dot(eye);
		m.m30 = 0;			m.m31 = 0;			m.m32 = 0;			m.m33 = 1;

		return m;
	}

	Mat4x4f matrix_util::CreateOrtho(float w, float h, float zn, float zf)
	{
		Mat4x4f m;

		m.m00 = 2.0f / w;	m.m01 = 0;			m.m02 = 0;					m.m03 = 0;
		m.m10 = 0;			m.m11 = 2.0f / h;	m.m12 = 0;					m.m13 = 0;
		m.m20 = 0;			m.m21 = 0;			m.m22 = 1.0f / (zn - zf);	m.m23 = zn / (zn - zf);
		m.m30 = 0;			m.m31 = 0;			m.m32 = 0;					m.m33 = 1;

		return m;
	}

	Mat4x4f matrix_util::CreateOrthoOffCenter(float l, float r, float b, float t, float zn, float zf)
	{
		Mat4x4f m;

		m.m00 = 2.0f / (r - l);	m.m01 = 0;				m.m02 = 0;					m.m03 = (l + r) / (l - r);
		m.m10 = 0;				m.m11 = 2.0f / (t - b);	m.m12 = 0;					m.m13 = (t + b) / (b - t);
		m.m20 = 0;				m.m21 = 0;				m.m22 = 1.0f / (zn - zf);	m.m23 = zn / (zn - zf);
		m.m30 = 0;				m.m31 = 0;				m.m32 = 0;					m.m33 = 1;

		return m;
	}

	Mat4x4f matrix_util::CreatePerspectiveFov(float fovy, float aspect, float zn, float zf)
	{
		// Perspective projection [Real-Time Rendering, 4.69, page 96]

		Mat4x4f m;

		float y_scale = 1.0f / math::Tan(fovy / 2.0f);
		float x_scale = y_scale / aspect;

		m.m00 = x_scale;	m.m01 = 0;			m.m02 = 0;				m.m03 = 0;
		m.m10 = 0;			m.m11 = y_scale;	m.m12 = 0;				m.m13 = 0;
		m.m20 = 0;			m.m21 = 0;			m.m22 = zf / (zn - zf);		m.m23 = zn*zf / (zn - zf);
		m.m30 = 0;			m.m31 = 0;			m.m32 = -1.0f;			m.m33 = 0;

		return m;
	}

} // namespace sb



#endif // _MATH_MATRIXUTIL_H
