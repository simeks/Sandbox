// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "MeshCompiler.h"
#include "Collada.h"

#include <Engine/Rendering/Mesh.h>
#include <Foundation/IO/InputBuffer.h>
#include <Foundation/IO/FileInputBuffer.h>

namespace sb
{

	MeshCompiler::MeshCompiler(const ConfigValue& config)
		: CompilerSystem::Compiler(config)
	{
	}
	MeshCompiler::~MeshCompiler()
	{
	}

	CompilerSystem::Result MeshCompiler::Compile(const FilePath& source_file, const FilePath& target_file, const CompilerSystem::CompilerContext& context)
	{
		FileStreamPtr file = context.asset_source->OpenFile(source_file.c_str(), File::READ);
		if (!file.Get() || file->Length() < 0)
		{
			SetError("Could not open file.");
			return CompilerSystem::FAILED;
		}

		FileInputBuffer buffer(file);
		file.Reset();


		collada::ColladaFile collada;
		if (!collada.Parse((const char*)buffer.Ptr(), (uint32_t)buffer.Length()))
		{
			SetError("Failed to parse collada file");
			return CompilerSystem::FAILED;
		}

		geometry::Mesh mesh;
		collada.Convert(mesh);

		geometry::CalculateTangents(mesh);
		geometry::CompressMesh(mesh);

		MeshData mesh_data;
		geometry::Convert(mesh, mesh_data);

		// Also create material resource files in the same folder as the mesh, given they doesn't already exist
		//	As the material files will be stored in the same folder as the mesh we need to modify the material name.

		for (uint32_t i = 0; i < mesh.sub_meshes.size(); ++i)
		{
			FilePath material_file_path = source_file;
			material_file_path.TrimExtension();
			material_file_path += "_";
			material_file_path += mesh.sub_meshes[i].material;

			// We also need to add the prefixed material to the mesh data
			mesh_data.materials[i] = material_file_path.c_str();
			material_file_path += ".material";
			FileStreamPtr material_file = context.asset_source->OpenFile(material_file_path.c_str(), File::READ);
			if (!material_file.Get())
			{
				material_file = context.asset_source->OpenFile(material_file_path.c_str(), File::WRITE);
			}
		}

		FilePath mesh_name = source_file;
		mesh_name.TrimExtension();
		mesh_data.name = mesh_name.c_str();

		vector<uint8_t> data;
		DynamicMemoryStream stream(&data);
		mesh_resource::Compile(mesh_data, stream);

		if (!WriteAsset(context.asset_target, target_file, data.data(), (uint32_t)data.size()))
			return CompilerSystem::FAILED;

		return CompilerSystem::SUCCESSFUL;
	}

} // namespace sb

