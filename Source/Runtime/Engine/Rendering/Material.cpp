// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Material.h"
#include "Texture.h"
#include "MaterialManager.h"
#include "ShaderManager.h"

#include <Foundation/Filesystem/File.h>
#include <Foundation/Container/ConfigValue.h>

namespace sb
{

	Material::Material(StringId32 name, MaterialData* data)
		: _data(data),
		_name(name),
		_shader(nullptr),
		_shader_context(nullptr),
		_initialized(false)
	{
	}
	Material::~Material()
	{
		if (_shader && _shader_context)
		{
			_shader->ReleaseContext(_shader_context);
		}
	}

	void Material::Initialize(ShaderManager* shader_manager, ResourceManager* resource_manager)
	{
		if (_initialized) // Already initialized
			return;

		_shader = shader_manager->GetShader(_data->shader);
		Assert(_shader);

		_shader_context = _shader->CreateContext();

		// Copy over variable data from template
		_shader_params.ConstructFrom(_data->shader_variables, _data->shader_variable_data);

		for (auto& entry : _data->textures)
		{
			Texture* texture = (Texture*)resource_manager->GetResource("texture", entry.second);
			SetTexture(entry.first, texture->GetRenderResource());
		}

		_initialized = true;
	}
	bool Material::Initialized() const
	{
		return _initialized;
	}

	void Material::SetScalar(StringId32 name, float value)
	{
		Assert(_initialized);
		_shader_params.SetScalar(name, value);
	}
	void Material::SetVector2(StringId32 name, const Vec2f& value)
	{
		Assert(_initialized);
		_shader_params.SetVector2(name, value);
	}
	void Material::SetVector3(StringId32 name, const Vec3f& value)
	{
		Assert(_initialized);
		_shader_params.SetVector3(name, value);
	}
	void Material::SetMatrix4x4(StringId32 name, const Mat4x4f& value)
	{
		Assert(_initialized);
		_shader_params.SetMatrix4x4(name, value);
	}

	void Material::SetTexture(StringId32 channel, const RTexture& texture)
	{
		_shader_params.SetResource(channel, texture);
	}

	StringId32 Material::GetName() const
	{
		return _name;
	}
	MaterialData* Material::GetData()
	{
		return _data;
	}
	Shader* Material::GetShader()
	{
		Assert(_initialized);
		return _shader;
	}
	ShaderContext* Material::GetShaderContext()
	{
		Assert(_initialized);
		return _shader_context;
	}
	const ShaderParameters& Material::GetShaderParams() const
	{
		Assert(_initialized);
		return _shader_params;
	}

	//-------------------------------------------------------------------------------

	MaterialData::MaterialData()
	{
	}

	//-------------------------------------------------------------------------------

	void material_resource::Compile(const MaterialData& data, Stream& stream)
	{
		uint32_t version = MATERIAL_RESOURCE_VERSION;
		stream.Write(&version, 4);

		stream.Write(&data.name, sizeof(StringId32));

		uint32_t tc = (uint32_t)data.textures.size();
		stream.Write(&tc, 4);
		if (tc)
		{
			stream.Write(data.textures.data(), tc*sizeof(pair<StringId32, StringId64>));
		}
		stream.Write(&data.shader, sizeof(StringId32));

		uint32_t vc = (uint32_t)data.shader_variables.size();
		stream.Write(&vc, 4);
		if (vc)
		{
			stream.Write(data.shader_variables.data(), vc*sizeof(ShaderVariable));
		}

		uint32_t vd = (uint32_t)data.shader_variable_data.size();
		stream.Write(&vd, 4);
		if (vd)
		{
			stream.Write(data.shader_variable_data.data(), vd);
		}
	}

	//-------------------------------------------------------------------------------

	void material_resource::Load(ResourceLoader::LoadContext& context)
	{
		uint32_t version;

		// Material version
		context.file->Read(&version, 4);
		if (version != MATERIAL_RESOURCE_VERSION)
		{
			logging::Error("Failed loading material: Wrong version, tried loading version %d, current version is %d.", version, MATERIAL_RESOURCE_VERSION);
		}

		MaterialData* data = new MaterialData();

		// Read name
		context.file->Read(&data->name, sizeof(StringId32));

		// Read textures
		uint32_t tc;
		context.file->Read(&tc, 4);
		if (tc)
		{
			data->textures.resize(tc);
			context.file->Read(data->textures.data(), sizeof(pair<StringId32, StringId64>)*tc);
		}

		// Read shader name
		context.file->Read(&data->shader, sizeof(StringId32));

		// Read shader variables
		uint32_t vc;
		context.file->Read(&vc, 4);
		if (vc)
		{
			data->shader_variables.resize(vc);
			Assert(data->shader_variables.data());
			context.file->Read(data->shader_variables.data(), vc*sizeof(ShaderVariable));
		}

		uint32_t vd;
		context.file->Read(&vd, 4);
		if (vd)
		{
			data->shader_variable_data.resize(vd);
			context.file->Read(data->shader_variable_data.data(), vd);
		}

		Material* material = new Material(data->name, data);

		// Store result
		context.result = material;
	}
	void material_resource::Unload(ResourceLoader::UnloadContext& context)
	{
		Material* material = (Material*)context.resource_data;
		MaterialData* material_data = material->GetData();

		delete material_data;
		delete material;
	}
	void material_resource::BringIn(void* user_data, void* resource_data)
	{
		Material* material = (Material*)resource_data;

		MaterialManager* material_manager = (MaterialManager*)user_data;
		material_manager->AddMaterial(material);
	}
	void material_resource::BringOut(void* user_data, void* resource_data)
	{
		Material* material = (Material*)resource_data;

		MaterialManager* material_manager = (MaterialManager*)user_data;
		material_manager->RemoveMaterial(material->GetName());
	}

	//-------------------------------------------------------------------------------

	void material_resource::RegisterResourceType(ResourceManager* resource_manager, MaterialManager* material_manager)
	{
		ResourceType resource_type;
		resource_type.bring_in_callback = BringIn;
		resource_type.bring_out_callback = BringOut;
		resource_type.load_callback = Load;
		resource_type.unload_callback = Unload;
		resource_type.user_data = material_manager;

		resource_manager->RegisterType("material", resource_type);
	}
	void material_resource::UnregisterResourceType(ResourceManager* resource_manager)
	{
		resource_manager->UnregisterType("material");
	}

	//-------------------------------------------------------------------------------

} // namespace sb

