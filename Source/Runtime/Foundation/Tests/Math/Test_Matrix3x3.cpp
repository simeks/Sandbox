// Copyright 2008-2014 Simon Ekström

#include "Testing/Framework.h"

#include <Foundation/Math/Matrix3x3.h>
#include <Foundation/Math/Vec3.h>

using namespace sb;

#define ASSERT_MATRIX3X3_EQ(m1, m2) \
	ASSERT_EQUAL_F(m1.m00, m2.m00, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m01, m2.m01, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m02, m2.m02, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m10, m2.m10, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m11, m2.m11, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m12, m2.m12, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m20, m2.m20, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m21, m2.m21, FLT_EPSILON); \
	ASSERT_EQUAL_F(m1.m22, m2.m22, FLT_EPSILON);


TEST_CASE(Matrix3x3_Invert)
{
	Mat3x3f mat(1, 1, 1, 
				1, 2, 2,
				1, 2, 3);

	mat.Invert();

	Mat3x3f mat_expected(2, -1, 0, 
						-1, 2, -1,
						0, -1, 1);

	ASSERT_MATRIX3X3_EQ(mat, mat_expected);
}

TEST_CASE(Matrix3x3_Determinant)
{
	Mat3x3f mat(1, 1, 1, 
				1, 2, 2,
				1, 2, 3);

	ASSERT_EQUAL_F(mat.Determinant(), 1.0f, FLT_EPSILON);
}

TEST_CASE(Matrix3x3_Mult)
{
	Mat3x3f mat1(1, 1, 1, 
				1, 2, 2,
				1, 2, 3);

	Mat3x3f mat2(3, 3, 3, 
				2, 2, 2,
				1, 1, 1);
	
	Mat3x3f mat = mat1 * mat2;
	
	Mat3x3f mat_expected1(6, 6, 6, 
						9, 9, 9,
						10, 10, 10);

	ASSERT_MATRIX3X3_EQ(mat, mat_expected1);

	mat = mat2 * mat1;
	
	Mat3x3f mat_expected2(9, 15, 18, 
						6, 10, 12,
						3, 5, 6);

	ASSERT_MATRIX3X3_EQ(mat, mat_expected2);

}

TEST_CASE(Matrix3x3_Vec3Mult)
{
	Mat3x3f mat(1, 1, 1, 
				1, 2, 2,
				1, 2, 3);

	Vec3f v(1, 1, 1);

	v = mat * v;

	ASSERT_EQUAL_F(v.x, 3, FLT_EPSILON);
	ASSERT_EQUAL_F(v.y, 5, FLT_EPSILON);
	ASSERT_EQUAL_F(v.z, 6, FLT_EPSILON);

}
