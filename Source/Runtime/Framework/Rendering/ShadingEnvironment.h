// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_SHADINGENVIRONMENT_H__
#define __RENDERING_SHADINGENVIRONMENT_H__

#include <Foundation/Math/Matrix4x4.h>
#include <Foundation/Math/Vec2.h>
#include <Foundation/Math/Vec3.h>

#include <Engine/Rendering/ShaderParameters.h>


namespace sb
{

	class ConfigValue;

	/// ShadingEnvironment is used for handling global variables in the shader system
	class ShadingEnvironment
	{
	public:
		ShadingEnvironment();
		~ShadingEnvironment();

		void Load(const ConfigValue& cfg);

		void SetScalar(StringId32 name, float value);
		void SetVector2(StringId32 name, const Vec2f& value);
		void SetVector3(StringId32 name, const Vec3f& value);
		void SetMatrix4x4(StringId32 name, const Mat4x4f& value);

		float GetScalar(StringId32 name) const;
		Vec2f GetVector2(StringId32 name) const;
		Vec3f GetVector3(StringId32 name) const;
		Mat4x4f GetMatrix4x4(StringId32 name) const;

		ShaderParameters& GetShaderParameters();

	private:
		ShaderParameters _shader_parameters;


	};

} // namespace sb


#endif // __RENDERING_SHADINGENVIRONMENT_H__

