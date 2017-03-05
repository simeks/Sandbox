// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_SHADERPARAMETERS_H__
#define __RENDERING_SHADERPARAMETERS_H__

#include "ShaderVariable.h"

#include <Engine/Rendering/RenderResource.h>

#include <Foundation/Math/Matrix4x4.h>
#include <Foundation/Math/Vec2.h>
#include <Foundation/Math/Vec3.h>
#include <Foundation/Math/Vec4.h>

namespace sb
{

	class ShaderParameters
	{
	public:
		typedef map<StringId32, ShaderVariable> VariableMap;
		typedef map<StringId32, RenderResource> ResourceMap;

		ShaderParameters();
		~ShaderParameters();

		/// Constructs this object for holding the specified variables.
		/// @remark This will remove any variables currently set.
		void ConstructFrom(const vector<ShaderVariable>& variable_reflection, const vector<uint8_t>& variable_data);

		void SetResource(StringId32 name, const RenderResource& resource);

		void SetScalar(StringId32 name, float value);
		void SetVector2(StringId32 name, const Vec2f& value);
		void SetVector3(StringId32 name, const Vec3f& value);
		void SetVector4(StringId32 name, const Vec4f& value);
		void SetMatrix4x4(StringId32 name, const Mat4x4f& value);

		void SetVector2Array(StringId32 name, const Vec2f* values, uint32_t count);
		void SetVector3Array(StringId32 name, const Vec3f* values, uint32_t count);
		void SetVector4Array(StringId32 name, const Vec4f* values, uint32_t count);
		void SetMatrix4x4Array(StringId32 name, const Mat4x4f* values, uint32_t count);

		float GetScalar(StringId32 name) const;
		Vec2f GetVector2(StringId32 name) const;
		Vec3f GetVector3(StringId32 name) const;
		Vec4f GetVector4(StringId32 name) const;
		Mat4x4f GetMatrix4x4(StringId32 name) const;

		bool HasVariable(StringId32 name) const;


		const VariableMap& GetVariableMap() const;
		const vector<uint8_t>& GetVariableData() const;

		const ResourceMap& GetResourceMap() const;

	private:
		VariableMap::iterator AddVariable(StringId32 name, ShaderVariable::Class var_class, uint32_t elements = 1);

		VariableMap _variable_reflection;
		vector<uint8_t> _variable_data;

		ResourceMap _resources;
	};

} // namespace sb

#endif // __RENDERING_SHADERPARAMETERS_H__
