// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Mesh.h"

#include <Foundation/Filesystem/File.h>
#include <Engine/Rendering/RenderDevice.h>
#include <Engine/Rendering/RenderResourceAllocator.h>

namespace sb
{

	//-------------------------------------------------------------------------------
	MeshData::MeshData()
	{
	}
	MeshData::~MeshData()
	{
	}

	//-------------------------------------------------------------------------------
	Mesh::Mesh(MeshData* data)
		: _data(data)
	{
	}
	Mesh::~Mesh()
	{
	}

	MeshData* Mesh::GetData()
	{
		return _data;
	}
	//-------------------------------------------------------------------------------

	void mesh_resource::Compile(MeshData& data, Stream& stream)
	{
		uint32_t version = MESH_RESOURCE_VERSION;
		stream.Write(&version, 4);

		stream.Write(&data.name, sizeof(StringId32));

		// Buffer descriptions
		stream.Write(&data.vertex_buffer.GetDesc(), sizeof(VertexBufferDesc));
		stream.Write(&data.index_buffer.GetDesc(), sizeof(IndexBufferDesc));

		data.vertex_declaration.Serialize(stream);


		// Bounding volume
		stream.Write(&data.bounding_box, sizeof(AABB));

		// Write vertex data
		uint32_t buffer_size = (uint32_t)data.vertex_data.size();
		stream.Write(&buffer_size, 4);
		if (buffer_size)
			stream.Write(data.vertex_data.data(), buffer_size);

		// Index data
		buffer_size = (uint32_t)data.index_data.size();
		stream.Write(&buffer_size, 4);
		if (buffer_size)
			stream.Write(data.index_data.data(), buffer_size);

		// Materials
		uint32_t material_count = (uint32_t)data.materials.size();
		stream.Write(&material_count, 4);
		for (uint32_t i = 0; i < material_count; ++i)
		{
			stream.Write(&data.materials[i], sizeof(StringId32));
		}

		// Submeshes
		uint32_t submesh_count = (uint32_t)data.submeshes.size();
		stream.Write(&submesh_count, 4);
		if (submesh_count)
			stream.Write(data.submeshes.data(), submesh_count * sizeof(MeshData::SubMesh));


	}

	void mesh_resource::Load(ResourceLoader::LoadContext& context)
	{
		uint32_t version;
		context.file->Read(&version, 4);
		Assert(version == MESH_RESOURCE_VERSION);

		MeshData* mesh_data = new MeshData();
		Assert(mesh_data);

		context.file->Read(&mesh_data->name, sizeof(StringId32));

		uint32_t s = sizeof(RenderResource);
		s;

		// Buffer descriptions
		VertexBufferDesc vb_desc;
		context.file->Read(&vb_desc, sizeof(VertexBufferDesc));
		mesh_data->vertex_buffer = RVertexBuffer(vb_desc);
		
		IndexBufferDesc ib_desc;
		context.file->Read(&ib_desc, sizeof(IndexBufferDesc));
		mesh_data->index_buffer = RIndexBuffer(ib_desc);

		mesh_data->vertex_declaration.Deserialize(*context.file);

		// Bounding volume
		context.file->Read(&mesh_data->bounding_box, sizeof(AABB));

		// Write vertex data
		uint32_t buffer_size;
		context.file->Read(&buffer_size, 4);
		mesh_data->vertex_data.resize(buffer_size);
		if (buffer_size)
			context.file->Read(mesh_data->vertex_data.data(), buffer_size);

		// Index data
		context.file->Read(&buffer_size, 4);
		mesh_data->index_data.resize(buffer_size);
		if (buffer_size)
			context.file->Read(mesh_data->index_data.data(), buffer_size);

		// Materials
		uint32_t material_count;
		context.file->Read(&material_count, 4);
		mesh_data->materials.resize(material_count);
		context.file->Read(mesh_data->materials.data(), material_count*sizeof(StringId32));

		// Submeshes
		uint32_t submesh_count;
		context.file->Read(&submesh_count, 4);
		mesh_data->submeshes.resize(submesh_count);
		if (submesh_count)
			context.file->Read(mesh_data->submeshes.data(), submesh_count * sizeof(MeshData::SubMesh));


		RenderResourceAllocator* render_resource_allocator = ((RenderDevice*)context.user_data)->GetResourceAllocator();

		render_resource_allocator->AllocateVertexBuffer(mesh_data->vertex_buffer, mesh_data->vertex_data.data());
		render_resource_allocator->AllocateIndexBuffer(mesh_data->index_buffer, mesh_data->index_data.data());
		render_resource_allocator->AllocateVertexDeclaration(mesh_data->vertex_declaration);

		context.result = new Mesh(mesh_data);
	}
	void mesh_resource::Unload(ResourceLoader::UnloadContext& context)
	{
		Mesh* mesh = (Mesh*)context.resource_data;
		MeshData* mesh_data = mesh->GetData();

		RenderResourceAllocator* render_resource_allocator = ((RenderDevice*)context.user_data)->GetResourceAllocator();

		render_resource_allocator->ReleaseResource(mesh_data->vertex_buffer);
		render_resource_allocator->ReleaseResource(mesh_data->index_buffer);
		render_resource_allocator->ReleaseResource(mesh_data->vertex_declaration);

		delete mesh_data;
		delete mesh;
	}

	void mesh_resource::RegisterResourceType(ResourceManager* resource_manager, RenderDevice* render_device)
	{
		ResourceType resource_type;
		resource_type.load_callback = Load;
		resource_type.unload_callback = Unload;
		resource_type.user_data = render_device;
		resource_manager->RegisterType("mesh", resource_type);
	}
	void mesh_resource::UnregisterResourceType(ResourceManager* resource_manager)
	{
		resource_manager->UnregisterType("mesh");
	}

	//-------------------------------------------------------------------------------


} // namespace sb
