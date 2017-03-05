// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Geometry.h"

#include <Foundation/Math/Vec3.h>
#include <Foundation/Hash/murmur_hash.h>


#ifndef assert
#define assert Assert
#endif

#include "TangentSpaceCalculation.h"

namespace sb
{

	namespace
	{
		/// Returns the element size in bytes for the specific semantic
		uint32_t SemanticSize(geometry::Stream::Channel::Semantic semantic)
		{
			switch (semantic)
			{
			case geometry::Stream::Channel::POSITION:
			case geometry::Stream::Channel::NORMAL:
			case geometry::Stream::Channel::TANGENT:
			case geometry::Stream::Channel::BINORMAL:
				return 3 * sizeof(float);
			case geometry::Stream::Channel::TEXCOORD:
				return 2 * sizeof(float);
			case geometry::Stream::Channel::COLOR:
				return 4 * sizeof(float);
			default:
			case geometry::Stream::Channel::UNKNOWN:
				return 0;
			};
		}


		/// Converts geometry::Stream::Channel::Semantic to VertexDeclaration semantic and type
		void GetSemanticAndType(geometry::Stream::Channel::Semantic semantic,
			RVertexDeclaration::ElementSemantic& out_semantic, RVertexDeclaration::ElementType& out_type)
		{
			switch (semantic)
			{
			case geometry::Stream::Channel::POSITION:
				out_semantic = RVertexDeclaration::ES_POSITION;
				out_type = RVertexDeclaration::ET_FLOAT3;
				return;
			case geometry::Stream::Channel::NORMAL:
				out_semantic = RVertexDeclaration::ES_NORMAL;
				out_type = RVertexDeclaration::ET_FLOAT3;
				return;
			case geometry::Stream::Channel::TEXCOORD:
				out_semantic = RVertexDeclaration::ES_TEXCOORD;
				out_type = RVertexDeclaration::ET_FLOAT2;
				return;
			case geometry::Stream::Channel::COLOR:
				out_semantic = RVertexDeclaration::ES_COLOR;
				out_type = RVertexDeclaration::ET_FLOAT4;
				return;
			case geometry::Stream::Channel::TANGENT:
				out_semantic = RVertexDeclaration::ES_TANGENT;
				out_type = RVertexDeclaration::ET_FLOAT3;
				return;
			case geometry::Stream::Channel::BINORMAL:
				out_semantic = RVertexDeclaration::ES_BINORMAL;
				out_type = RVertexDeclaration::ET_FLOAT3;
				return;
			case geometry::Stream::Channel::UNKNOWN:
				Assert(false);
			};
		}
	};


	namespace geometry
	{
		struct TriangleInputProxy
		{
			geometry::Mesh& mesh;

			struct StreamProxy
			{
				int stream_index; // -1 => No stream
				uint32_t stream_offset;
			};

			StreamProxy position_stream;
			StreamProxy normal_stream;
			StreamProxy texcoord_stream;


			TriangleInputProxy(geometry::Mesh& mesh);

			//! not virtual to save the call overhead
			//! /return 0..
			unsigned int GetTriangleCount() const;

			//! not virtual to save the call overhead
			//! /param indwTriNo 0..
			//! /param outdwPos
			//! /param outdwNorm
			//! /param outdwUV
			void GetTriangleIndices(const unsigned int indwTriNo, unsigned int outdwPos[3], unsigned int outdwNorm[3], unsigned int outdwUV[3]) const;

			//! not virtual to save the call overhead
			//! /param indwPos 0..
			//! /param outfPos
			void GetPos(const unsigned int indwPos, float outfPos[3]) const;

			//! not virtual to save the call overhead
			//! /param indwPos 0..
			//! /param outfUV 
			void GetUV(const unsigned int indwPos, float outfUV[2]) const;

		private:
			const TriangleInputProxy& operator=(const TriangleInputProxy&) { return *this; }

		};


		TriangleInputProxy::TriangleInputProxy(geometry::Mesh& mesh)
			: mesh(mesh)
		{
			position_stream.stream_index = normal_stream.stream_index = texcoord_stream.stream_index = -1;

			vector<Stream>::iterator stream_it, stream_end;
			stream_it = mesh.streams.begin(); stream_end = mesh.streams.end();
			for (uint32_t stream_index = 0; stream_it != stream_end; ++stream_it, ++stream_index)
			{
				vector<Stream::Channel>::iterator channel_it, channel_end;
				channel_it = stream_it->channels.begin(); channel_end = stream_it->channels.end();
				for (; channel_it != channel_end; ++channel_it)
				{
					switch (channel_it->semantic)
					{
					case geometry::Stream::Channel::POSITION:
						if (position_stream.stream_index == -1) // We only support one stream of this semantic so ignore any additional streams
						{
							position_stream.stream_index = stream_index;
							position_stream.stream_offset = channel_it->offset;
						}
						break;
					case geometry::Stream::Channel::NORMAL:
						if (normal_stream.stream_index == -1) // We only support one stream of this semantic so ignore any additional streams
						{
							normal_stream.stream_index = stream_index;
							normal_stream.stream_offset = channel_it->offset;

						}
						break;
					case geometry::Stream::Channel::TEXCOORD:
						if (texcoord_stream.stream_index == -1) // We only support one stream of this semantic so ignore any additional streams
						{
							texcoord_stream.stream_index = stream_index;
							texcoord_stream.stream_offset = channel_it->offset;
						}
						break;
					};

				}

			}
		}


		unsigned int TriangleInputProxy::GetTriangleCount() const
		{
			Assert(mesh.prim_type == geometry::Mesh::TRIANGLE_LIST); // Not supported
			return (mesh.index_count / 3);
		}

		void TriangleInputProxy::GetTriangleIndices(const unsigned int indwTriNo, unsigned int outdwPos[3], unsigned int outdwNorm[3], unsigned int outdwUV[3]) const
		{
			uint32_t index = indwTriNo * 3;
			if (position_stream.stream_index != -1)
			{
				outdwPos[0] = mesh.index_streams[position_stream.stream_index].indices[index];
				outdwPos[1] = mesh.index_streams[position_stream.stream_index].indices[index + 1];
				outdwPos[2] = mesh.index_streams[position_stream.stream_index].indices[index + 2];
			}
			if (normal_stream.stream_index != -1)
			{
				outdwNorm[0] = mesh.index_streams[normal_stream.stream_index].indices[index];
				outdwNorm[1] = mesh.index_streams[normal_stream.stream_index].indices[index + 1];
				outdwNorm[2] = mesh.index_streams[normal_stream.stream_index].indices[index + 2];
			}
			if (texcoord_stream.stream_index != -1)
			{
				outdwUV[0] = mesh.index_streams[texcoord_stream.stream_index].indices[index];
				outdwUV[1] = mesh.index_streams[texcoord_stream.stream_index].indices[index + 1];
				outdwUV[2] = mesh.index_streams[texcoord_stream.stream_index].indices[index + 2];
			}
		}

		void TriangleInputProxy::GetPos(const unsigned int indwPos, float outfPos[3]) const
		{
			if (position_stream.stream_index != -1)
			{
				memory::Memcpy(outfPos, &mesh.streams[position_stream.stream_index].data[indwPos*sizeof(float)* 3 + position_stream.stream_offset],
					mesh.streams[position_stream.stream_index].stride);
			}
		}

		void TriangleInputProxy::GetUV(const unsigned int indwPos, float outfUV[2]) const
		{
			if (texcoord_stream.stream_index != -1)
			{
				memory::Memcpy(outfUV, &mesh.streams[texcoord_stream.stream_index].data[indwPos*sizeof(float)* 2 + texcoord_stream.stream_offset],
					mesh.streams[texcoord_stream.stream_index].stride);
			}
		}

	}; // namespace geometry

	void geometry::CalculateTangents(geometry::Mesh& mesh)
	{
		Assert(mesh.prim_type == Mesh::TRIANGLE_LIST);

		TriangleInputProxy::StreamProxy tangent_stream;
		tangent_stream.stream_index = -1;
		TriangleInputProxy::StreamProxy binormal_stream;
		binormal_stream.stream_index = -1;

		// Check if there already is any tangent or binormal streams
		vector<Stream>::iterator stream_it, stream_end;
		stream_it = mesh.streams.begin(); stream_end = mesh.streams.end();
		for (uint32_t stream_index = 0; stream_it != stream_end; ++stream_it, ++stream_index)
		{
			vector<Stream::Channel>::iterator channel_it, channel_end;
			channel_it = stream_it->channels.begin(); channel_end = stream_it->channels.end();
			for (; channel_it != channel_end; ++channel_it)
			{
				if (channel_it->semantic == Stream::Channel::TANGENT)
				{
					tangent_stream.stream_index = stream_index;
					tangent_stream.stream_offset = channel_it->offset;
				}
				else if (channel_it->semantic == Stream::Channel::BINORMAL)
				{
					binormal_stream.stream_index = stream_index;
					binormal_stream.stream_offset = channel_it->offset;
				}
			}
		}


		if (tangent_stream.stream_index == -1)
		{
			tangent_stream.stream_index = (int)mesh.streams.size();
			Stream stream;
			stream.stride = 3 * sizeof(float);

			Stream::Channel channel;
			channel.semantic = Stream::Channel::TANGENT;
			channel.sem_index = 0;
			channel.offset = 0;
			stream.channels.push_back(channel);
			mesh.streams.push_back(stream);

			mesh.index_streams.push_back(IndexStream());
		}
		else
		{
			mesh.index_streams[tangent_stream.stream_index].indices.clear();
			mesh.streams[tangent_stream.stream_index].data.clear();
		}

		if (binormal_stream.stream_index == -1)
		{
			binormal_stream.stream_index = (uint32_t)mesh.streams.size();
			Stream stream;
			stream.stride = 3 * sizeof(float);

			Stream::Channel channel;
			channel.semantic = Stream::Channel::BINORMAL;
			channel.sem_index = 0;
			channel.offset = 0;
			stream.channels.push_back(channel);
			mesh.streams.push_back(stream);

			mesh.index_streams.push_back(IndexStream());
		}
		else
		{
			mesh.index_streams[binormal_stream.stream_index].indices.clear();
			mesh.streams[binormal_stream.stream_index].data.clear();
		}


		TriangleInputProxy proxy(mesh);

		CTangentSpaceCalculation<TriangleInputProxy> tanget_calc;
		if (tanget_calc.CalculateTangentSpace(proxy) != 0)
		{
			logging::Warning("Failed to calculate tangents for mesh");
			return;
		}


		uint32_t tricount = mesh.index_count / 3;

		// Set indicies
		for (uint32_t i = 0; i < tricount; ++i)
		{
			uint32_t dwBase[3];
			tanget_calc.GetTriangleBaseIndices(i, dwBase);

			mesh.index_streams[tangent_stream.stream_index].indices.insert(
				mesh.index_streams[tangent_stream.stream_index].indices.end(),
				dwBase, dwBase + 3);
			mesh.index_streams[binormal_stream.stream_index].indices.insert(
				mesh.index_streams[binormal_stream.stream_index].indices.end(),
				dwBase, dwBase + 3);

		}

		// Set data
		for (uint32_t i = 0; i < tanget_calc.GetBaseCount(); ++i)
		{
			float normal[3];
			float tangent[3];
			float binormal[3];

			tanget_calc.GetBase(i, tangent, binormal, normal);

			mesh.streams[tangent_stream.stream_index].data.insert(
				mesh.streams[tangent_stream.stream_index].data.end(),
				(uint8_t*)tangent,
				(uint8_t*)(tangent + 3));

			mesh.streams[binormal_stream.stream_index].data.insert(
				mesh.streams[binormal_stream.stream_index].data.end(),
				(uint8_t*)binormal,
				(uint8_t*)(binormal + 3));

		}

	}

	void geometry::CalculateBounds(const geometry::Mesh& mesh, AABB& bounding_box)
	{
		uint32_t stream_index = 0;

		bounding_box.max.x = -FLT_MAX;
		bounding_box.max.x = -FLT_MAX;
		bounding_box.max.x = -FLT_MAX;

		bounding_box.min.x = FLT_MAX;
		bounding_box.min.x = FLT_MAX;
		bounding_box.min.x = FLT_MAX;

		// Check if there already is any tangent or binormal streams
		vector<Stream>::const_iterator stream_it, stream_end;
		stream_it = mesh.streams.begin(); stream_end = mesh.streams.end();
		for (; stream_it != stream_end; ++stream_it, ++stream_index)
		{
			vector<Stream::Channel>::const_iterator channel_it, channel_end;
			channel_it = stream_it->channels.begin(); channel_end = stream_it->channels.end();
			for (; channel_it != channel_end; ++channel_it)
			{
				if (channel_it->semantic == Stream::Channel::POSITION)
				{
					uint32_t vert_count = uint32_t(stream_it->data.size() / stream_it->stride);
					for (uint32_t v = 0; v < vert_count; ++v)
					{
						float vec[3];
						memory::Memcpy(vec, &stream_it->data[(v*stream_it->stride) + channel_it->offset], sizeof(float)* 3);

						bounding_box.max.x = Max(bounding_box.max.x, vec[0]);
						bounding_box.max.y = Max(bounding_box.max.y, vec[1]);
						bounding_box.max.z = Max(bounding_box.max.z, vec[2]);

						bounding_box.min.x = Min(bounding_box.min.x, vec[0]);
						bounding_box.min.y = Min(bounding_box.min.y, vec[1]);
						bounding_box.min.z = Min(bounding_box.min.z, vec[2]);
					}
				}
			}
		}
	}

	void geometry::CompressMesh(geometry::Mesh& mesh)
	{
		Stream new_stream;
		IndexStream new_index_stream;

		map<uint64_t, uint32_t> hash_to_index; // Maps a vertex hash to a index for the index stream

		uint32_t vertex_size = 0; // For holding the total size (in bytes) of the final vertex

		vector<Stream>::iterator stream_it, stream_end;
		stream_it = mesh.streams.begin(); stream_end = mesh.streams.end();
		for (; stream_it != stream_end; ++stream_it)
		{
			vector<Stream::Channel>::iterator channel_it, channel_end;
			channel_it = stream_it->channels.begin(); channel_end = stream_it->channels.end();
			for (; channel_it != channel_end; ++channel_it)
			{
				new_stream.channels.push_back(*channel_it);
				new_stream.channels.back().offset = vertex_size;
				vertex_size += SemanticSize(channel_it->semantic);
			}
		}

		new_stream.stride = vertex_size;

		LinearAllocatorWithBuffer<128> temp_alloc;
		uint8_t* tmp_vertex = (uint8_t*)temp_alloc.Allocate(vertex_size);

		uint32_t new_index = 0;
		for (uint32_t i = 0; i < mesh.index_count; ++i)
		{
			uint32_t tmp_offset = 0; // Offset for filling the temp vertex
			// Build the vertex for this index
			for (uint32_t stream_idx = 0; stream_idx < mesh.streams.size(); ++stream_idx)
			{
				Stream& data_stream = mesh.streams[stream_idx];
				IndexStream& index_stream = mesh.index_streams[stream_idx];

				uint32_t old_index = index_stream.indices[i];

				memory::Memcpy(tmp_vertex + tmp_offset, &data_stream.data[old_index*data_stream.stride], data_stream.stride);
				tmp_offset += data_stream.stride;
			}

			// Find any duplicate versions of the newly built vertex
			uint64_t h = murmur_hash_64(tmp_vertex, vertex_size, 0);
			map<uint64_t, uint32_t>::iterator map_it = hash_to_index.find(h);
			if (map_it != hash_to_index.end())
			{
				// Duplicate found
				new_index_stream.indices.push_back(map_it->second);
				continue;
			}

			// Insert vertex
			uint32_t offset = (uint32_t)new_stream.data.size();
			new_stream.data.resize(offset + vertex_size);
			memory::Memcpy(new_stream.data.data() + offset, tmp_vertex, vertex_size);

			new_index_stream.indices.push_back(new_index);
			// Add to map
			hash_to_index[h] = new_index;

			++new_index;
		}

		mesh.streams.clear();
		mesh.index_streams.clear();

		mesh.streams.push_back(new_stream);
		mesh.index_streams.push_back(new_index_stream);
	}



	bool geometry::Convert(const geometry::Mesh& mesh, MeshData& mesh_data)
	{
		// In MeshData the data is assumed to be merged into a single vertex stream (or rather vertex stream + index stream)
		if (mesh.streams.size() != 1 || mesh.index_streams.size() != 1)
		{
			return false;
		}

		// Build vertex declaration
		mesh_data.vertex_declaration.Clear();

		for (auto& channel : mesh.streams[0].channels)
		{
			RVertexDeclaration::Element element;
			element.slot = 0;
			GetSemanticAndType(channel.semantic, element.semantic, element.type);
			element.sem_index = channel.sem_index;

			mesh_data.vertex_declaration.AddElement(element);
		}

		uint32_t vertex_buffer_size = (uint32_t)mesh.streams[0].data.size();

		VertexBufferDesc vb_desc;
		vb_desc.vertex_size = mesh.streams[0].stride;
		vb_desc.vertex_count = vertex_buffer_size / mesh.streams[0].stride;
		vb_desc.usage = hardware_buffer::IMMUTABLE;
		mesh_data.vertex_buffer = RVertexBuffer(vb_desc);

		IndexBufferDesc ib_desc;
		ib_desc.index_format = index_buffer::INDEX_32; // TODO: Is it worth using 16-bit for smaller meshes?
		ib_desc.index_count = mesh.index_count;
		ib_desc.usage = hardware_buffer::IMMUTABLE;
		mesh_data.index_buffer = RIndexBuffer(ib_desc);

		// Copy vertex and index data
		mesh_data.vertex_data.resize(vertex_buffer_size);
		memory::Memcpy(mesh_data.vertex_data.data(), mesh.streams[0].data.data(), vertex_buffer_size);

		mesh_data.index_data.resize(mesh.index_count * sizeof(uint32_t));
		memory::Memcpy(mesh_data.index_data.data(), mesh.index_streams[0].indices.data(), mesh.index_count * sizeof(uint32_t));

		uint32_t sub_mesh_offset = 0;

		for (auto& sub_mesh : mesh.sub_meshes)
		{
			MeshData::SubMesh sub_mesh_data;
			sub_mesh_data.material_index = (uint32_t)mesh_data.materials.size();
			sub_mesh_data.offset = sub_mesh_offset;
			sub_mesh_data.size = sub_mesh.index_count;
			sub_mesh_offset += sub_mesh.index_count;

			mesh_data.materials.push_back(sub_mesh.material);
			mesh_data.submeshes.push_back(sub_mesh_data);
		}

		CalculateBounds(mesh, mesh_data.bounding_box);

		return true;
	}

} // namespace sb
