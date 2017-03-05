// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_MATERIAL_H__
#define __RENDERING_MATERIAL_H__

#include "Shader.h"
#include "ShaderParameters.h"

#include <Foundation/Resource/ResourceManager.h>
#include <Foundation/Math/Matrix4x4.h>
#include <Foundation/Math/Vec2.h>

namespace sb
{

	class Texture;
	class ResourceManager;
	class ShaderManager;
	class MaterialManager;
	class RenderResource;
	class RTexture;

	struct MaterialData
	{
		StringId32 name;

		vector<pair<StringId32, StringId64>> textures; // Textures <channel name, texture name> 
		StringId32 shader;

		vector<ShaderVariable> shader_variables;
		vector<uint8_t> shader_variable_data;

		MaterialData();
	};

	class Material
	{
	public:
		Material(StringId32 name, MaterialData* data);
		~Material();

		/// Initializes the material, mainly linking any necessary dependencies like textures and shaders.
		void Initialize(ShaderManager* shader_manager, ResourceManager* resource_manager);

		/// Returns true if the material is fully initalized, false if not
		bool Initialized() const;

		void SetScalar(StringId32 name, float value);
		void SetVector2(StringId32 name, const Vec2f& value);
		void SetVector3(StringId32 name, const Vec3f& value);
		void SetMatrix4x4(StringId32 name, const Mat4x4f& value);

		/// @brief Sets the texture for the specified channel.
		void SetTexture(StringId32 channel, const RTexture& texture);

		StringId32 GetName() const;

		MaterialData* GetData();
		const MaterialData* GetData() const;

		Shader* GetShader();
		ShaderContext* GetShaderContext();
		const ShaderParameters& GetShaderParams() const;

	private:
		MaterialData* _data;

		StringId32 _name;

		Shader* _shader;
		ShaderParameters _shader_params;
		ShaderContext* _shader_context;

		bool _initialized;
	};


	namespace material_resource
	{
		enum { MATERIAL_RESOURCE_VERSION = 3 };

		/// Compiles a material resource from a config file
		void Compile(const MaterialData& data, Stream& stream);

		void Load(ResourceLoader::LoadContext& context);
		void Unload(ResourceLoader::UnloadContext& context);
		void BringIn(void* user_data, void* resource_data);
		void BringOut(void* user_data, void* resource_data);

		void RegisterResourceType(ResourceManager* resource_manager, MaterialManager* material_manager);
		void UnregisterResourceType(ResourceManager* resource_manager);

	};

} // namespace sb


#endif // __RENDERING_MATERIAL_H__