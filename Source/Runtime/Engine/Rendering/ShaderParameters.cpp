// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "ShaderParameters.h"
#include "Shader.h"

namespace sb
{

	//-------------------------------------------------------------------------------
	ShaderParameters::ShaderParameters()
	{
	}
	ShaderParameters::~ShaderParameters()
	{
	}
	//-------------------------------------------------------------------------------
	void ShaderParameters::ConstructFrom(const vector<ShaderVariable>& variable_reflection, const vector<uint8_t>& variable_data)
	{
		// Clear old state
		_variable_reflection.clear();
		_variable_data.clear();

		if (variable_reflection.empty())
			return;

		for (auto& var : variable_reflection)
		{
			_variable_reflection[var.name] = var;
		}

		_variable_data.resize(variable_data.size());
		memcpy(_variable_data.data(), variable_data.data(), variable_data.size());
	}
	//-------------------------------------------------------------------------------
	void ShaderParameters::SetResource(StringId32 name, const RenderResource& resource)
	{
		_resources[name] = resource;
	}
	//-------------------------------------------------------------------------------
	void ShaderParameters::SetScalar(StringId32 name, float value)
	{
		VariableMap::iterator it = _variable_reflection.find(name);
		if (it == _variable_reflection.end())
		{
			// New variable
			it = AddVariable(name, ShaderVariable::SCALAR);
		}

		shader_variable::BindScalar(_variable_data.data(), it->second.offset, value);
	}
	void ShaderParameters::SetVector2(StringId32 name, const Vec2f& value)
	{
		VariableMap::iterator it = _variable_reflection.find(name);
		if (it == _variable_reflection.end())
		{
			// New variable
			it = AddVariable(name, ShaderVariable::VECTOR2);
		}

		shader_variable::BindVector2(_variable_data.data(), it->second.offset, value);
	}
	void ShaderParameters::SetVector3(StringId32 name, const Vec3f& value)
	{
		VariableMap::iterator it = _variable_reflection.find(name);
		if (it == _variable_reflection.end())
		{
			// New variable
			it = AddVariable(name, ShaderVariable::VECTOR3);
		}

		shader_variable::BindVector3(_variable_data.data(), it->second.offset, value);
	}
	void ShaderParameters::SetVector4(StringId32 name, const Vec4f& value)
	{
		VariableMap::iterator it = _variable_reflection.find(name);
		if (it == _variable_reflection.end())
		{
			// New variable
			it = AddVariable(name, ShaderVariable::VECTOR4);
		}

		shader_variable::BindVector4(_variable_data.data(), it->second.offset, value);
	}
	void ShaderParameters::SetMatrix4x4(StringId32 name, const Mat4x4f& value)
	{
		VariableMap::iterator it = _variable_reflection.find(name);
		if (it == _variable_reflection.end())
		{
			// New variable
			it = AddVariable(name, ShaderVariable::MATRIX4X4);
		}
		shader_variable::BindMatrix4x4(_variable_data.data(), it->second.offset, value);
	}

	void ShaderParameters::SetVector2Array(StringId32 name, const Vec2f* values, uint32_t count)
	{
		VariableMap::iterator it = _variable_reflection.find(name);
		if (it == _variable_reflection.end())
		{
			// New variable
			it = AddVariable(name, ShaderVariable::VECTOR2_ARRAY, count);
		}
		Assert(it->second.elements >= count);
		Assert(it->second.var_class == ShaderVariable::VECTOR2_ARRAY);

		shader_variable::BindVector2Array(_variable_data.data(), it->second.offset, values, count);
	}
	void ShaderParameters::SetVector3Array(StringId32 name, const Vec3f* values, uint32_t count)
	{
		VariableMap::iterator it = _variable_reflection.find(name);
		if (it == _variable_reflection.end())
		{
			// New variable
			it = AddVariable(name, ShaderVariable::VECTOR3_ARRAY, count);
		}
		Assert(it->second.elements >= count);
		Assert(it->second.var_class == ShaderVariable::VECTOR3_ARRAY);

		shader_variable::BindVector3Array(_variable_data.data(), it->second.offset, values, count);
	}
	void ShaderParameters::SetVector4Array(StringId32 name, const Vec4f* values, uint32_t count)
	{
		VariableMap::iterator it = _variable_reflection.find(name);
		if (it == _variable_reflection.end())
		{
			// New variable
			it = AddVariable(name, ShaderVariable::VECTOR4_ARRAY, count);
		}
		Assert(it->second.elements >= count);
		Assert(it->second.var_class == ShaderVariable::VECTOR4_ARRAY);

		shader_variable::BindVector4Array(_variable_data.data(), it->second.offset, values, count);
	}

	void ShaderParameters::SetMatrix4x4Array(StringId32 name, const Mat4x4f* values, uint32_t count)
	{
		VariableMap::iterator it = _variable_reflection.find(name);
		if (it == _variable_reflection.end())
		{
			// New variable
			it = AddVariable(name, ShaderVariable::MATRIX4X4_ARRAY, count);
		}
		Assert(it->second.elements >= count);
		Assert(it->second.var_class == ShaderVariable::MATRIX4X4_ARRAY);

		shader_variable::BindMatrix4x4Array(_variable_data.data(), it->second.offset, values, count);
	}

	//-------------------------------------------------------------------------------
	float ShaderParameters::GetScalar(StringId32 name) const
	{
		VariableMap::const_iterator it = _variable_reflection.find(name);
		Assert(it != _variable_reflection.end());
		Assert(it->second.var_class == ShaderVariable::SCALAR);

		float ret;
		memcpy(&ret, memory::PointerAdd(_variable_data.data(), it->second.offset), sizeof(float));

		return ret;
	}
	Vec2f ShaderParameters::GetVector2(StringId32 name) const
	{
		VariableMap::const_iterator it = _variable_reflection.find(name);
		Assert(it != _variable_reflection.end());
		Assert(it->second.var_class == ShaderVariable::VECTOR2);

		Vec2f ret;
		memcpy(&ret, memory::PointerAdd(_variable_data.data(), it->second.offset), sizeof(Vec2f));

		return ret;
	}
	Vec3f ShaderParameters::GetVector3(StringId32 name) const
	{
		VariableMap::const_iterator it = _variable_reflection.find(name);
		Assert(it != _variable_reflection.end());
		Assert(it->second.var_class == ShaderVariable::VECTOR3);

		Vec3f ret;
		memcpy(&ret, memory::PointerAdd(_variable_data.data(), it->second.offset), sizeof(Vec3f));

		return ret;
	}
	Vec4f ShaderParameters::GetVector4(StringId32 name) const
	{
		VariableMap::const_iterator it = _variable_reflection.find(name);
		Assert(it != _variable_reflection.end());
		Assert(it->second.var_class == ShaderVariable::VECTOR4);

		Vec4f ret;
		memcpy(&ret, memory::PointerAdd(_variable_data.data(), it->second.offset), sizeof(Vec4f));

		return ret;
	}
	Mat4x4f ShaderParameters::GetMatrix4x4(StringId32 name) const
	{
		VariableMap::const_iterator it = _variable_reflection.find(name);
		Assert(it != _variable_reflection.end());
		Assert(it->second.var_class == ShaderVariable::MATRIX4X4);

		Mat4x4f ret;
		memcpy(&ret, memory::PointerAdd(_variable_data.data(), it->second.offset), sizeof(Mat4x4f));

		return ret;
	}

	//-------------------------------------------------------------------------------
	bool ShaderParameters::HasVariable(StringId32 name) const
	{
		VariableMap::const_iterator it = _variable_reflection.find(name);
		if (it != _variable_reflection.end())
			return true;
		return false;
	}
	//-------------------------------------------------------------------------------
	const ShaderParameters::VariableMap& ShaderParameters::GetVariableMap() const
	{
		return _variable_reflection;
	}
	const vector<uint8_t>& ShaderParameters::GetVariableData() const
	{
		return _variable_data;
	}
	const ShaderParameters::ResourceMap& ShaderParameters::GetResourceMap() const
	{
		return _resources;
	}
	//-------------------------------------------------------------------------------
	ShaderParameters::VariableMap::iterator ShaderParameters::AddVariable(StringId32 name, ShaderVariable::Class var_class, uint32_t elements)
	{
		VariableMap::iterator it = _variable_reflection.find(name);
		if (it != _variable_reflection.end())
			return it;

		Assert(var_class != ShaderVariable::UNKNOWN);

		ShaderVariable var;
		var.elements = elements;
		var.name = name;
		var.offset = (uint32_t)_variable_data.size();
		var.var_class = var_class;

		_variable_data.insert(_variable_data.end(), shader_variable::GetSize(var.var_class)*var.elements, 0);

		return _variable_reflection.insert(pair<StringId32, ShaderVariable>(name, var)).first;
	}

	//-------------------------------------------------------------------------------

} // namespace sb

