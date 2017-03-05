// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "ShadingEnvironment.h"

#include <Foundation/Container/ConfigValue.h>


namespace sb
{

	ShadingEnvironment::ShadingEnvironment()
	{
	}
	ShadingEnvironment::~ShadingEnvironment()
	{
	}

	void ShadingEnvironment::Load(const ConfigValue& cfg)
	{
		if (!cfg["variables"].IsArray())
			return;

		for (uint32_t v = 0; v < cfg["variables"].Size(); ++v)
		{
			const ConfigValue& var = cfg["variables"][v];
			Assert(var["name"].IsString());
			Assert(var["type"].IsString());

			StringId32 name = var["name"].AsString();
			if (strcmp(var["type"].AsString(), "scalar") == 0)
			{
				float scalar;
				if (shader_variable::ParseScalar(var["value"], scalar))
				{
					SetScalar(name, scalar);
				}
				else
				{
					logging::Warning("Failed to set variable '%s', value has an invalid format.", var["name"].AsString());
				}
			}
			else if (strcmp(var["type"].AsString(), "vector2") == 0)
			{
				Vec2f vec;
				if (shader_variable::ParseVector2(var["value"], vec))
				{
					SetVector2(name, vec);
				}
				else
				{
					logging::Warning("Failed to set variable '%s', value has an invalid format.", var["name"].AsString());
				}
			}
			else if (strcmp(var["type"].AsString(), "vector3") == 0)
			{
				Vec3f vec;
				if (shader_variable::ParseVector3(var["value"], vec))
				{
					SetVector3(name, vec);
				}
				else
				{
					logging::Warning("Failed to set variable '%s', value has an invalid format.", var["name"].AsString());
				}
			}
			else if (strcmp(var["type"].AsString(), "matrix4x4") == 0)
			{
				Mat4x4f mat;
				if (shader_variable::ParseMatrix4x4(var["value"], mat))
				{
					SetMatrix4x4(name, mat);
				}
				else
				{
					logging::Warning("Failed to set variable '%s', value has an invalid format.", var["name"].AsString());
				}
			}
			else
			{
				logging::Warning("ShadingEnvironment: Variable ('%s') type '%s' not recognized.", var["name"].AsString(), var["type"].AsString());
			}
		}
	}


	void ShadingEnvironment::SetScalar(StringId32 name, float value)
	{
		_shader_parameters.SetScalar(name, value);
	}
	void ShadingEnvironment::SetVector2(StringId32 name, const Vec2f& value)
	{
		_shader_parameters.SetVector2(name, value);
	}
	void ShadingEnvironment::SetVector3(StringId32 name, const Vec3f& value)
	{
		_shader_parameters.SetVector3(name, value);
	}
	void ShadingEnvironment::SetMatrix4x4(StringId32 name, const Mat4x4f& value)
	{
		_shader_parameters.SetMatrix4x4(name, value);
	}

	float ShadingEnvironment::GetScalar(StringId32 name) const
	{
		return _shader_parameters.GetScalar(name);
	}
	Vec2f ShadingEnvironment::GetVector2(StringId32 name) const
	{
		return _shader_parameters.GetVector2(name);
	}
	Vec3f ShadingEnvironment::GetVector3(StringId32 name) const
	{
		return _shader_parameters.GetVector3(name);
	}
	Mat4x4f ShadingEnvironment::GetMatrix4x4(StringId32 name) const
	{
		return _shader_parameters.GetMatrix4x4(name);
	}

	ShaderParameters& ShadingEnvironment::GetShaderParameters()
	{
		return _shader_parameters;
	}

} // namespace sb

