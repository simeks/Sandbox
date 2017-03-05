// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Collada.h"

#include <Foundation/tinyxml2/tinyxml2.h>

namespace sb
{

	namespace
	{
		geometry::Stream::Channel::Semantic StringToSemantic(const string& str)
		{
			if (str == "POSITION")
				return geometry::Stream::Channel::POSITION;
			else if (str == "NORMAL")
				return geometry::Stream::Channel::NORMAL;
			else if (str == "TEXCOORD")
				return geometry::Stream::Channel::TEXCOORD;
			else if (str == "COLOR")
				return geometry::Stream::Channel::COLOR;
			else if (str == "TANGENT" || str == "TEXTANGENT")
				return geometry::Stream::Channel::TANGENT;
			else if (str == "BINORMAL" || str == "TEXBINORMAL")
				return geometry::Stream::Channel::BINORMAL;
			else
				return geometry::Stream::Channel::UNKNOWN;

		}
	};


	collada::ColladaFile::ColladaFile()
	{

	}
	collada::ColladaFile::~ColladaFile()
	{

	}

	void collada::ColladaFile::Convert(geometry::Mesh& mesh)
	{
		if (!_geometry.size())
		{
			logging::Warning("ColladaFile: No geometry to convert");
			return;
		}

		mesh.streams.clear();
		mesh.index_streams.clear();
		mesh.sub_meshes.clear();

		Geometry& geometry = _geometry[0];
		map<string, uint32_t> source_name_to_index; // Used for mapping sources to array indexes (in Mesh object)


		// Collada is currently always assumed to be triangles
		mesh.prim_type = geometry::Mesh::TRIANGLE_LIST;


		for (auto& source : geometry.sources)
		{
			source_name_to_index[source.id] = (uint32_t)mesh.streams.size();
			mesh.streams.push_back(geometry::Stream());
			geometry::Stream& stream = mesh.streams.back();

			stream.stride = source.stride * sizeof(float); // source.stride is number of floats per vertex, convert to bytes
			stream.data.resize(source.data.size() * sizeof(float));
			memory::Memcpy(stream.data.data(), source.data.data(), stream.data.size());

			// One index stream for each vertex stream
			mesh.index_streams.push_back(geometry::IndexStream());
		}
		mesh.index_count = 0;


		for (auto& triangles : geometry.triangle_sets)
		{
			// We start by separating all the channels and their indices as collada have mashed them all into one
			// In collada it looks like this:
			// Indices: 1 | 1 | 2 | 2 | ... | n1 | n2 
			// Channel: 1,  2,  1,  2   ...   1,   2	// Shows which channel owns each index

			uint32_t skip = (uint32_t)triangles.inputs.size(); // Number of indices to skip when iterating through indices for a specific input

			for (auto& input : triangles.inputs)
			{
				uint32_t index = source_name_to_index[input.source];
				geometry::IndexStream& index_stream = mesh.index_streams[index];
				geometry::Stream& data_stream = mesh.streams[index];

				for (uint32_t idx = input.offset; idx < triangles.indices.size(); idx += skip)
				{
					index_stream.indices.push_back(triangles.indices[idx]);
				}

				// Fill in channel info
				geometry::Stream::Channel channel;
				channel.offset = 0;
				channel.sem_index = input.set;

				if (input.semantic == "VERTEX")
				{
					Assert(geometry.vertices.id == input.source); // Only supports one vertex stream
					if (geometry.vertices.inputs.size() > 1)
					{
						logging::Warning("ColladaFile: Currently only supports one stream in <vertice>"); // TODO: Need more?
					}
					channel.semantic = StringToSemantic(geometry.vertices.inputs[0].semantic);
				}
				else
				{
					channel.semantic = StringToSemantic(input.semantic);
				}

				if (channel.semantic == geometry::Stream::Channel::UNKNOWN)
				{
					logging::Warning("Unknown channel semantic for mesh.");
					return;
				}

				// Avoid duplicated channels
				bool found = false;
				for (uint32_t c = 0; c < data_stream.channels.size(); ++c)
				{
					if (data_stream.channels[c].offset == channel.offset &&
						data_stream.channels[c].semantic == channel.semantic &&
						data_stream.channels[c].sem_index == channel.sem_index)
					{
						found = true;
						break;
					}
				}

				if (!found)
				{
					data_stream.channels.push_back(channel);
				}
			}

			geometry::SubMesh sub_mesh;
			sub_mesh.material = _materials[triangles.material_id];
			sub_mesh.index_count = uint32_t(triangles.indices.size() / triangles.inputs.size()); // Calculate how many indices that belong to this submesh

			mesh.sub_meshes.push_back(sub_mesh);

			mesh.index_count += sub_mesh.index_count; // Always 3 indices per triangle
		}

	}

	bool collada::ColladaFile::Parse(const char* data, uint32_t len)
	{
		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError error = doc.Parse(data, len);
		if (error != tinyxml2::XML_NO_ERROR)
		{
			logging::Warning("ColladaFile: XMLError (error = %d) str1 = %s str2 = %s", error, doc.GetErrorStr1(), doc.GetErrorStr2());
			return false;
		}

		tinyxml2::XMLElement* root = doc.RootElement();
		Assert(strcmp(root->Name(), "COLLADA") == 0);

		tinyxml2::XMLElement* asset = root->FirstChildElement("asset");
		if (asset)
		{
			tinyxml2::XMLElement* up_axis = asset->FirstChildElement("up_axis");
			if (up_axis)
			{
				if (strcmp(up_axis->GetText(), "Z_UP") != 0)
				{
					logging::Warning("ColladaFile: Up-axis setting for collada file: %s, engine uses Z-axis as Up-axis.", up_axis->GetText());
				}
			}
		}

		// Parse material library
		tinyxml2::XMLElement* material_library = root->FirstChildElement("library_materials");
		tinyxml2::XMLElement* material_elem = material_library->FirstChildElement("material");
		while (material_elem)
		{
			const char* material_id = material_elem->Attribute("id");
			const char* material_name = material_elem->Attribute("name");
			if (!material_id || !material_name)
				continue;

			_materials[material_id] = material_name;

			material_elem = material_elem->NextSiblingElement("material");
		}




		tinyxml2::XMLElement* geometry_library = root->FirstChildElement("library_geometries");
		if (!ParseGeometryLibrary(geometry_library))
			return false;

		if (_geometry.size() > 1)
		{
			logging::Warning("ColladaFile: Converter is only supporting one geometry object per collada file at this moment");
		}

		return true;
	}


	bool collada::ColladaFile::ParseGeometryLibrary(tinyxml2::XMLElement* element)
	{
		tinyxml2::XMLElement* geometry = element->FirstChildElement("geometry");
		while (geometry)
		{
			if (!ParseGeometry(geometry))
				return false;
			geometry = geometry->NextSiblingElement("geometry");
		}

		return true;
	}
	bool collada::ColladaFile::ParseGeometry(tinyxml2::XMLElement* element)
	{
		Geometry geometry;

		tinyxml2::XMLElement* mesh = element->FirstChildElement("mesh");
		if (mesh)
		{
			// Parse sources
			tinyxml2::XMLElement* source_elem = mesh->FirstChildElement("source");
			while (source_elem)
			{
				Source source;
				source.id = source_elem->Attribute("id");

				tinyxml2::XMLElement* data_elem = source_elem->FirstChildElement("float_array");
				if (!data_elem)
				{
					logging::Warning("Expected float_array.");
					return false;
				}

				uint32_t count = (uint32_t)atoi(data_elem->Attribute("count"));
				const char* data = data_elem->GetText();
				for (uint32_t i = 0; i < count; ++i)
				{
					if (*data == 0)
					{
						logging::Warning("Expected more values in float_array");
						return false;
					}

					float v = (float)atof(data);
					source.data.push_back(v);

					while (*data != ' ' && *data != '\0') ++data;
					if (*data != '\0')
						++data;
				}

				tinyxml2::XMLElement* accessor_elem = source_elem->FirstChildElement("technique_common")->FirstChildElement("accessor");
				if (accessor_elem)
				{
					source.stride = atoi(accessor_elem->Attribute("stride"));
				}

				geometry.sources.push_back(source);

				source_elem = source_elem->NextSiblingElement("source");
			}

			// Parse vertices
			tinyxml2::XMLElement* vert_elem = mesh->FirstChildElement("vertices");
			if (vert_elem)
			{
				geometry.vertices.id = vert_elem->Attribute("id");
				ParseInputs(vert_elem, geometry.vertices.inputs);
			}


			// Parse triangles
			// We support both triangles and polylist, assuming that the polylist is triangulated


			tinyxml2::XMLElement* triangles_elem = mesh->FirstChildElement("triangles");
			while (triangles_elem)
			{
				TriangleSet triangle_set;

				triangle_set.count = atoi(triangles_elem->Attribute("count"));
				// Lookup material name from the id
				triangle_set.material_id = triangles_elem->Attribute("material");

				ParseInputs(triangles_elem, triangle_set.inputs);

				// Index count for collada: Number of triangles * 3 * number of input streams
				uint32_t index_count = triangle_set.count * (uint32_t)triangle_set.inputs.size() * 3; 
				tinyxml2::XMLElement* indices_elem = triangles_elem->FirstChildElement("p");

				const char* indices_data = indices_elem->GetText();
				for (uint32_t i = 0; i < index_count; ++i)
				{
					if (*indices_data == 0)
					{
						logging::Warning("Expected more values in <p>");
						return false;
					}

					uint32_t index = atoi(indices_data);
					triangle_set.indices.push_back(index);

					while (*indices_data != ' ' && *indices_data != '\0') ++indices_data;
					if (*indices_data != '\0')
						++indices_data;
				}
				geometry.triangle_sets.push_back(triangle_set);

				triangles_elem = triangles_elem->NextSiblingElement("triangles");
			}

			tinyxml2::XMLElement* polylist_elem = mesh->FirstChildElement("polylist");
			while (polylist_elem)
			{
				TriangleSet triangle_set;

				triangle_set.count = atoi(polylist_elem->Attribute("count"));
				triangle_set.material_id = polylist_elem->Attribute("material");

				ParseInputs(polylist_elem, triangle_set.inputs);

				tinyxml2::XMLElement* vcount = polylist_elem->FirstChildElement("vcount");
				const char* vcount_data = vcount->GetText();
				for (uint32_t i = 0; i < triangle_set.count; ++i)
				{
					if (*vcount_data == 0)
					{
						logging::Warning("Expected more values in <vcount>");
						return false;
					}
					uint32_t count = atoi(vcount_data);
					if (count != 3)
					{
						logging::Warning("Expected triangles in polylist, vcount = %d, should be 3", count);
						return false;
					}

					while (*vcount_data != ' ' && *vcount_data != '\0') ++vcount_data;
					if (*vcount_data != '\0')
						++vcount_data;
				}

				// Index count for collada: Number of triangles * 3 * number of input streams
				uint32_t index_count = triangle_set.count * (uint32_t)triangle_set.inputs.size() * 3; 
				tinyxml2::XMLElement* indices_elem = polylist_elem->FirstChildElement("p");

				const char* indices_data = indices_elem->GetText();
				for (uint32_t i = 0; i < index_count; ++i)
				{
					if (*indices_data == 0)
					{
						logging::Warning("Expected more values in <p>");
						return false;
					}

					uint32_t index = atoi(indices_data);
					triangle_set.indices.push_back(index);

					while (*indices_data != ' ' && *indices_data != '\0') ++indices_data;
					if (*indices_data != '\0')
						++indices_data;
				}
				geometry.triangle_sets.push_back(triangle_set);

				polylist_elem = polylist_elem->NextSiblingElement("polylist");
			}


		}

		_geometry.push_back(geometry);

		return true;
	}
	bool collada::ColladaFile::ParseInputs(tinyxml2::XMLElement* triangles_elem, vector<InputChannel>& inputs)
	{
		tinyxml2::XMLElement* input_elem = triangles_elem->FirstChildElement("input");
		while (input_elem)
		{
			InputChannel input;

			if (input_elem->Attribute("offset") != '\0')
				input.offset = atoi(input_elem->Attribute("offset"));
			else
				input.offset = 0;

			if (input_elem->Attribute("set") != '\0')
				input.set = atoi(input_elem->Attribute("set"));
			else
				input.set = 0;

			input.semantic = input_elem->Attribute("semantic");
			input.source = input_elem->Attribute("source");
			if (input.source[0] == '#') // Remove any leading number sign (For ids in collada files)
				input.source.erase(0, 1);

			inputs.push_back(input);

			input_elem = input_elem->NextSiblingElement("input");
		}
		return true;
	}

} // namespace sb
