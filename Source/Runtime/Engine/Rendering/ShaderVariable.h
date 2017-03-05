// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_SHADERVARIABLE_H__
#define __RENDERING_SHADERVARIABLE_H__

#include <Foundation/Math/Vec2.h>
#include <Foundation/Math/Vec3.h>
#include <Foundation/Math/Vec4.h>
#include <Foundation/Math/Matrix3x3.h>
#include <Foundation/Math/Matrix4x4.h>

namespace sb
{

	class ConfigValue;

	struct ShaderVariable
	{
		enum Class
		{
			UNKNOWN,
			SCALAR,
			VECTOR2,
			VECTOR3,
			VECTOR4,
			MATRIX4X4,
			SCALAR_ARRAY,
			VECTOR2_ARRAY,
			VECTOR3_ARRAY,
			VECTOR4_ARRAY,
			MATRIX4X4_ARRAY
		};
		StringId32 name;

		uint32_t offset;
		Class var_class;
		uint32_t elements;
	};

	namespace shader_variable
	{
		/// Returns the size of the specified variable class,
		///	returns the size of one element if class is specified as an array.
		uint32_t GetSize(ShaderVariable::Class var_class);

		/// Bind a variable to the specified array with the specified offset.
		void BindScalar(void* dest, const uint32_t offset, const float scalar);

		/// Bind a variable to the specified array with the specified offset.
		void BindVector2(void* dest, const uint32_t offset, const Vec2f& vec);

		/// Bind a variable to the specified array with the specified offset.
		void BindVector3(void* dest, const uint32_t offset, const Vec3f& vec);

		/// Bind a variable to the specified array with the specified offset.
		void BindVector4(void* dest, const uint32_t offset, const Vec4f& vec);

		/// Bind a variable to the specified array with the specified offset.
		void BindMatrix4x4(void* dest, const uint32_t offset, const Mat4x4f& mat);


		/// Bind a variable to the specified array with the specified offset.
		void BindVector2Array(void* dest, const uint32_t offset, const Vec2f* values, const uint32_t count);

		/// Bind a variable to the specified array with the specified offset.
		void BindVector3Array(void* dest, const uint32_t offset, const Vec3f* values, const uint32_t count);

		/// Bind a variable to the specified array with the specified offset.
		void BindVector4Array(void* dest, const uint32_t offset, const Vec4f* values, const uint32_t count);

		/// Bind a variable to the specified array with the specified offset.
		void BindMatrix4x4Array(void* dest, const uint32_t offset, const Mat4x4f* values, const uint32_t count);



		/// Parses the value from a JSON object
		/// @param value The resulting value, should be considered invalid if conversion fails.
		/// @return True if the conversion was successful, false if not.
		bool ParseScalar(const ConfigValue& object, float& value);

		/// Parses the value from a JSON object
		/// @param value The resulting value, should be considered invalid if conversion fails.
		/// @return True if the conversion was successful, false if not.
		bool ParseVector2(const ConfigValue& object, Vec2f& value);

		/// Parses the value from a JSON object
		/// @param value The resulting value, should be considered invalid if conversion fails.
		/// @return True if the conversion was successful, false if not.
		bool ParseVector3(const ConfigValue& object, Vec3f& value);

		/// Parses the value from a JSON object
		/// @param value The resulting value, should be considered invalid if conversion fails.
		/// @return True if the conversion was successful, false if not.
		bool ParseVector4(const ConfigValue& object, Vec4f& value);

		/// Parses the value from a JSON object
		/// @param value The resulting value, should be considered invalid if conversion fails.
		/// @return True if the conversion was successful, false if not.
		bool ParseMatrix4x4(const ConfigValue& object, Mat4x4f& value);

	};


} // namespace sb

#endif // __RENDERING_SHADERVARIABLE_H__
