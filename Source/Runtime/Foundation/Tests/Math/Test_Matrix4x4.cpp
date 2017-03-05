// Copyright 2008-2014 Simon Ekström

#include "Testing/Framework.h"

#include <Foundation/Math/Matrix4x4.h>
#include <Foundation/Math/Vec4.h>

using namespace sb;

#define ASSERT_MATRIX4X4_EQ(m1, m2) \
	ASSERT_EQUAL_F(m1.m00, m2.m00, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m01, m2.m01, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m02, m2.m02, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m03, m2.m03, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m10, m2.m10, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m11, m2.m11, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m12, m2.m12, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m13, m2.m13, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m20, m2.m20, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m21, m2.m21, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m22, m2.m22, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m23, m2.m23, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m30, m2.m30, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m31, m2.m31, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m32, m2.m32, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m33, m2.m33, FLT_EPSILON);


TEST_CASE(Matrix4x4_Invert)
{
	Mat4x4f mat(1, 1, 1, 1, 
				1, 2, 2, 2,
				1, 2, 3, 3,
				1, 2, 3, 4);

	mat.Invert();

	Mat4x4f mat_expected(2, -1, 0, 0,
						-1, 2, -1, 0,
						0, -1, 2, -1,
						0, 0, -1, 1);

	ASSERT_MATRIX4X4_EQ(mat, mat_expected);
}

TEST_CASE(Matrix4x4_Determinant)
{
	Mat4x4f mat(1, 1, 1, 1, 
				1, 2, 2, 2,
				1, 2, 3, 3,
				1, 2, 3, 4);
	
	ASSERT_EQUAL_F(mat.Determinant(), 1.0f, FLT_EPSILON);
}

TEST_CASE(Matrix4x4_Mult)
{
	Mat4x4f mat1(1, 1, 1, 1, 
				2, 2, 2, 2,
				3, 3, 3, 3,
				4, 4, 4, 4);
	
	Mat4x4f mat2(1, 2, 3, 4, 
				5, 6, 7, 8,
				9, 10, 11, 12,
				13, 14, 15, 16);
	
	Mat4x4f mat = mat1 * mat2;

	Mat4x4f mat_expected1(28, 32, 36, 40, 
						56, 64, 72, 80,
						84, 96, 108, 120,
						112, 128, 144, 160);
	
	ASSERT_MATRIX4X4_EQ(mat, mat_expected1);

	mat = mat2 * mat1;

	Mat4x4f mat_expected2(30, 30, 30, 30, 
						70, 70, 70, 70,
						110, 110, 110, 110,
						150, 150, 150, 150);
	
	ASSERT_MATRIX4X4_EQ(mat, mat_expected2);
}

TEST_CASE(Matrix4x4_Vec4Mult)
{
	Mat4x4f mat(1, 2, 3, 4, 
				5, 6, 7, 8,
				9, 10, 11, 12,
				13, 14, 15, 16);

	Vec4f v(1, 1, 1, 1);

	v = mat * v;

	ASSERT_EQUAL_F(v.x, 10, FLT_EPSILON);
	ASSERT_EQUAL_F(v.y, 26, FLT_EPSILON);
	ASSERT_EQUAL_F(v.z, 42, FLT_EPSILON);
	ASSERT_EQUAL_F(v.w, 58, FLT_EPSILON);

}
