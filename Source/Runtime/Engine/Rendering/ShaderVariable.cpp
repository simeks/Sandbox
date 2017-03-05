// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "ShaderVariable.h"

#include <Foundation/Container/ConfigValue.h>

namespace sb
{

	uint32_t shader_variable::GetSize(ShaderVariable::Class var_class)
	{
		static uint32_t class_to_size[] = {
			0,					/* UNKNOWN */
			sizeof(float),		/* SCALAR */
			sizeof(float)* 2,	/* VECTOR2 */
			sizeof(float)* 3,	/* VECTOR3 */
			sizeof(float)* 4,	/* VECTOR4 */
			sizeof(float)* 16,	/* MATRIX4X4 */

			// Array elements are rounded to float4
			sizeof(float)* 4,		/* SCALAR_ARRAY */
			sizeof(float)* 4,	/* VECTOR2_ARRAY */
			sizeof(float)* 4,	/* VECTOR3_ARRAY */
			sizeof(float)* 4,	/* VECTOR4_ARRAY */
			sizeof(float)* 16,	/* MATRIX4X4_ARRAY */
		};
		return class_to_size[var_class];
	}

	void shader_variable::BindScalar(void* dest, const uint32_t offset, const float scalar)
	{
		memcpy(memory::PointerAdd(dest, offset), &scalar, sizeof(float));
	}

	void shader_variable::BindVector2(void* dest, const uint32_t offset, const Vec2f& vec)
	{
		memcpy(memory::PointerAdd(dest, offset), &vec, sizeof(float)* 2);
	}

	void shader_variable::BindVector3(void* dest, const uint32_t offset, const Vec3f& vec)
	{
		memcpy(memory::PointerAdd(dest, offset), &vec, sizeof(float)* 3);
	}

	void shader_variable::BindVector4(void* dest, const uint32_t offset, const Vec4f& vec)
	{
		memcpy(memory::PointerAdd(dest, offset), &vec, sizeof(float)* 4);
	}

	void shader_variable::BindMatrix4x4(void* dest, const uint32_t offset, const Mat4x4f& mat)
	{
		memcpy(memory::PointerAdd(dest, offset), &mat, sizeof(float)* 16);
	}


	void shader_variable::BindVector2Array(void* dest, const uint32_t offset, const Vec2f* values, const uint32_t count)
	{
		Assert(values);
		Assert(count);

		// We need to pad the values so that each element in the array is the size of float4
		uint32_t local_offset = offset;
		for (uint32_t i = 0; i < count; ++i)
		{
			memcpy(memory::PointerAdd(dest, local_offset), &values[i], sizeof(float)* 2);
			local_offset += sizeof(float)* 4;
		}
	}

	void shader_variable::BindVector3Array(void* dest, const uint32_t offset, const Vec3f* values, const uint32_t count)
	{
		Assert(values);
		Assert(count);

		// We need to pad the values so that each element in the array is the size of float4
		uint32_t local_offset = offset;
		for (uint32_t i = 0; i < count; ++i)
		{
			memcpy(memory::PointerAdd(dest, local_offset), &values[i], sizeof(float)* 3);
			local_offset += sizeof(float)* 4;
		}
	}

	void shader_variable::BindVector4Array(void* dest, const uint32_t offset, const Vec4f* values, const uint32_t count)
	{
		Assert(values);
		Assert(count);

		// We need to pad the values so that each element in the array is the size of float4
		uint32_t local_offset = offset;
		for (uint32_t i = 0; i < count; ++i)
		{
			memcpy(memory::PointerAdd(dest, local_offset), &values[i], sizeof(float)* 4);
			local_offset += sizeof(float)* 4;
		}
	}

	void shader_variable::BindMatrix4x4Array(void* dest, const uint32_t offset, const Mat4x4f* values, const uint32_t count)
	{
		Assert(values);
		Assert(count);

		memcpy(memory::PointerAdd(dest, offset), values, sizeof(float)* 16 * count);
	}


	bool shader_variable::ParseScalar(const ConfigValue& object, float& value)
	{
		if (object.IsNumber())
		{
			value = object.AsFloat();

			return true;
		}
		return false;
	}
	bool shader_variable::ParseVector2(const ConfigValue& object, Vec2f& value)
	{
		if (object.IsArray() && object.Size() == 2)
		{
			value = Vec2f(object[0].AsFloat(), object[1].AsFloat());

			return true;
		}
		return false;
	}
	bool shader_variable::ParseVector3(const ConfigValue& object, Vec3f& value)
	{
		if (object.IsArray() && object.Size() == 3)
		{
			value = Vec3f(object[0].AsFloat(), object[1].AsFloat(), object[2].AsFloat());

			return true;
		}
		return false;
	}
	bool shader_variable::ParseVector4(const ConfigValue& object, Vec4f& value)
	{
		if (object.IsArray() && object.Size() == 4)
		{
			value = Vec4f(object[0].AsFloat(), object[1].AsFloat(), object[2].AsFloat(), object[3].AsFloat());

			return true;
		}
		return false;
	}
	bool shader_variable::ParseMatrix4x4(const ConfigValue& object, Mat4x4f& value)
	{
		if (object.IsArray() && object.Size() == 4)
		{
			Mat4x4f mat;
			for (uint32_t y = 0; y < 4; ++y)
			{
				if (object[y].IsArray() && object[y].Size() == 4)
				{
					mat.SetRow(y, Vec4f(object[y][0].AsFloat(), object[y][1].AsFloat(), object[y][2].AsFloat(), object[y][3].AsFloat()));
				}
				else
				{
					return false;
				}
			}

			value = mat;
			return true;
		}
		return false;
	}

} // namespace sb

