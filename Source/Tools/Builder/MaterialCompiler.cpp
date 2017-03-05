// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "MaterialCompiler.h"
#include "DependencyDatabase.h"
#include "ShaderDatabase.h"

#include <Foundation/Container/ConfigValue.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Json/Json.h>

#include <Engine/Rendering/Material.h>

namespace sb
{

	MaterialCompiler::MaterialCompiler(const ConfigValue& config)
		: CompilerSystem::Compiler(config)
	{
	}
	MaterialCompiler::~MaterialCompiler()
	{
	}
	CompilerSystem::Result MaterialCompiler::Compile(const FilePath& source_file, const FilePath& target_file, const CompilerSystem::CompilerContext& context)
	{
		ConfigValue material_node;
		MaterialData material_data;

		simplified_json::Reader reader;
		if (!reader.ReadFile(context.asset_source, source_file.c_str(), material_node))
		{
			SetError(reader.GetErrorMessage().c_str());
			return CompilerSystem::FAILED;
		}

		ConfigValue& tex_node = material_node["textures"];
		if (tex_node.IsObject())
		{
			ConfigValue::ConstIterator it, end;
			it = tex_node.Begin(); end = tex_node.End();
			for (; it != end; ++it)
			{
				Assert(it->second.IsString());
				material_data.textures.push_back(pair<StringId32, StringId64>(it->first, it->second.AsString()));
			}

		}


		// Add any shader dependencies
		ConfigValue& shader_node = material_node["shader"];
		if (shader_node.IsObject())
		{
			vector<string> options;
			if (shader_node["options"].IsArray())
			{
				for (uint32_t i = 0; i < shader_node["options"].Size(); ++i)
				{
					if (!shader_node["options"][i].IsString()) continue;

					options.push_back(string(shader_node["options"][i].AsString()));
				}
			}

			if (shader_node["name"].IsString())
			{
				if (!context.shader_database->PreloadPermutation(shader_node["name"].AsString(), options))
				{
					SetError("Failed to preload shader.");
					return CompilerSystem::FAILED;
				}

				FilePath shader_path = shader_node["name"].AsString();
				shader_path += ".shader_src";

				context.dependency_database->AddDependent(shader_path.c_str(), source_file.c_str());

				string shader_name;
				context.shader_database->GetShader(shader_node["name"].AsString()).BuildName(options, shader_name);
				material_data.shader = shader_name;
			}

			if (shader_node["variables"].IsArray())
			{
				ParseShaderVariables(shader_node["variables"], material_data);
			}
		}
		else
		{
			logging::Warning("Material has not specified any shader");
		}

		FilePath material_name = source_file;
		material_name.TrimExtension();
		material_data.name = material_name.c_str();

		vector<uint8_t> data;
		DynamicMemoryStream stream(&data);
		material_resource::Compile(material_data, stream);

		if (!WriteAsset(context.asset_target, target_file, data.data(), (uint32_t)data.size()))
			return CompilerSystem::FAILED;

		return CompilerSystem::SUCCESSFUL;
	}
	void MaterialCompiler::ParseShaderVariables(const ConfigValue& variables, MaterialData& material_data)
	{
		if (!variables.IsArray())
			return;

		for (uint32_t v = 0; v < variables.Size(); ++v)
		{
			const ConfigValue& var = variables[v];
			if (!var["name"].IsString())
				continue;
			if (!var["type"].IsString())
				continue;

			ShaderVariable shader_var;
			shader_var.elements = 1;
			shader_var.name = var["name"].AsString();
			shader_var.offset = (uint32_t)material_data.shader_variable_data.size();

			if (strcmp(var["type"].AsString(), "scalar") == 0)
			{
				shader_var.var_class = ShaderVariable::SCALAR;
				material_data.shader_variable_data.insert(material_data.shader_variable_data.end(),
					shader_variable::GetSize(shader_var.var_class), 0);

				if (var["value"].IsNumber())
				{
					shader_variable::BindScalar(material_data.shader_variable_data.data(), shader_var.offset, var["value"].AsFloat());
				}
			}
			else if (strcmp(var["type"].AsString(), "vector2") == 0)
			{
				shader_var.var_class = ShaderVariable::VECTOR2;
				material_data.shader_variable_data.insert(material_data.shader_variable_data.end(),
					shader_variable::GetSize(shader_var.var_class), 0);

				if (var["value"].IsArray() && var["value"].Size() == 2 &&
					var["value"][0].IsNumber() && var["value"][1].IsNumber())
				{
					shader_variable::BindVector2(material_data.shader_variable_data.data(), shader_var.offset,
						Vec2f(var["value"][0].AsFloat(), var["value"][1].AsFloat()));
				}
			}
			else if (strcmp(var["type"].AsString(), "vector3") == 0)
			{
				shader_var.var_class = ShaderVariable::VECTOR3;
				material_data.shader_variable_data.insert(material_data.shader_variable_data.end(),
					shader_variable::GetSize(shader_var.var_class), 0);

				if (var["value"].IsArray() && var["value"].Size() == 3 &&
					var["value"][0].IsNumber() && var["value"][1].IsNumber() && var["value"][2].IsNumber())
				{
					shader_variable::BindVector3(material_data.shader_variable_data.data(), shader_var.offset,
						Vec3f(var["value"][0].AsFloat(), var["value"][1].AsFloat(), var["value"][2].AsFloat()));
				}
			}
			else if (strcmp(var["type"].AsString(), "vector4") == 0)
			{
				shader_var.var_class = ShaderVariable::VECTOR4;
				material_data.shader_variable_data.insert(material_data.shader_variable_data.end(),
					shader_variable::GetSize(shader_var.var_class), 0);

				if (var["value"].IsArray() && var["value"].Size() == 4 &&
					var["value"][0].IsNumber() && var["value"][1].IsNumber() && var["value"][2].IsNumber() && var["value"][3].IsNumber())
				{
					shader_variable::BindVector4(material_data.shader_variable_data.data(), shader_var.offset,
						Vec4f(var["value"][0].AsFloat(), var["value"][1].AsFloat(), var["value"][2].AsFloat(), var["value"][3].AsFloat()));
				}
			}
			else if (strcmp(var["type"].AsString(), "matrix4x4") == 0)
			{
				shader_var.var_class = ShaderVariable::MATRIX4X4;
				material_data.shader_variable_data.insert(
					material_data.shader_variable_data.end(),
					shader_variable::GetSize(shader_var.var_class), 0);

				if (var["value"].IsArray() && var["value"].Size() == 4)
				{
					Mat4x4f value = Mat4x4f::CreateIdentity();
					for (uint32_t y = 0; y < 4; ++y)
					{
						if (var["value"][y].IsArray() && var["value"][y].Size() == 4 &&
							var["value"][y][0].IsNumber() && var["value"][y][1].IsNumber() &&
							var["value"][y][2].IsNumber() && var["value"][y][3].IsNumber())
						{
							value.SetRow(y, Vec4f(var["value"][y][0].AsFloat(), var["value"][y][1].AsFloat(), var["value"][y][2].AsFloat(), var["value"][y][3].AsFloat()));
						}
					}
					shader_variable::BindMatrix4x4(material_data.shader_variable_data.data(), shader_var.offset, value);
				}
			}
			else
			{
				logging::Warning("MaterialCompiler: Variable type '%s' not recognized.", var["type"].AsString());
				continue;
			}

			material_data.shader_variables.push_back(shader_var);
		}
	}

} // namespace sb

