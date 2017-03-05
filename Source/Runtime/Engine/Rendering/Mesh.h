// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_MESH_H__
#define __RENDERING_MESH_H__

#include <Foundation/Resource/ResourceManager.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Math/AABB.h>

#include <Engine/Rendering/RVertexBuffer.h>
#include <Engine/Rendering/RIndexBuffer.h>
#include <Engine/Rendering/RVertexDeclaration.h>

namespace sb
{

	class RenderDevice;

	struct MeshData
	{
		struct SubMesh
		{
			uint32_t offset; // Offset in indices
			uint32_t size; // Number of indices belonging to this sub mesh
			uint32_t material_index;
		};

		StringId32 name;

		RVertexBuffer vertex_buffer;
		RIndexBuffer index_buffer;
		RVertexDeclaration vertex_declaration;

		AABB bounding_box;

		vector<uint8_t> vertex_data;
		vector<uint8_t> index_data;

		vector<StringId32> materials;
		vector<SubMesh> submeshes;

		MeshData();
		~MeshData();

	};

	class Mesh
	{
	public:
		Mesh(MeshData* data);
		~Mesh();


		MeshData* GetData();

	private:
		MeshData* _data;

	};

	namespace mesh_resource
	{
		enum { MESH_RESOURCE_VERSION = 4 };

		void Compile(MeshData& data, Stream& stream);

		void Load(ResourceLoader::LoadContext& context);
		void Unload(ResourceLoader::UnloadContext& context);
		void BringIn(void* user_data, void* resource_data);
		void BringOut(void* user_data, void* resource_data);

		void RegisterResourceType(ResourceManager* resource_manager, RenderDevice* render_device);
		void UnregisterResourceType(ResourceManager* resource_manager);
	};


} // namespace sb


#endif // __RENDERING_MESH_H__
