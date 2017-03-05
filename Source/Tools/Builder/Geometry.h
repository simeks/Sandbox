// Copyright 2008-2014 Simon Ekström

#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <Engine/Rendering/Mesh.h>

namespace sb
{

	namespace geometry
	{

		struct Stream
		{
			struct Channel
			{
				enum Semantic
				{
					POSITION,
					NORMAL,
					TEXCOORD,
					COLOR,
					TANGENT,
					BINORMAL,
					UNKNOWN
				};

				Semantic semantic;
				uint32_t sem_index; // Semantic index, mainly for texcoords
				uint32_t offset; // Offset in bytes
			};

			Stream() {}

			vector<Channel> channels;
			vector<uint8_t> data;
			uint32_t stride; // Number of bytes per vertex
		};

		struct IndexStream
		{
			IndexStream() {}

			vector<uint32_t> indices;
		};

		struct SubMesh
		{
			SubMesh() : index_count(0) {}

			string material;
			uint32_t index_count; // Number of indices in the index streams that belong to this sub mesh
		};

		struct Mesh
		{
			enum PrimitiveType
			{
				TRIANGLE_LIST
			};

			Mesh() {}

			vector<Stream> streams;
			vector<IndexStream> index_streams;

			PrimitiveType prim_type;
			uint32_t index_count; // Number of indices (e.g. 3 for a triangle, 6 for 2 triangles, etc)

			vector<SubMesh> sub_meshes;
		};


		/// Calculates the tangents for the specified mesh
		/// This assumes all streams for the mesh are split up, meaning one channel per stream (Position, normal, etc)
		void CalculateTangents(geometry::Mesh& mesh);

		/// Calculates the bounding volume for the specified mesh
		void CalculateBounds(const geometry::Mesh& mesh, AABB& bounding_box);

		/// Compresses the specified mesh, merging all streams and calculating new indices
		void CompressMesh(geometry::Mesh& mesh);

		/// Converts a Mesh-object into engine-format MeshData
		/// @return True if convertion was successful, false if failed
		bool Convert(const geometry::Mesh& mesh, MeshData& mesh_data);

	};

} // namespace sb


#endif // __GEOMETRY_H__
