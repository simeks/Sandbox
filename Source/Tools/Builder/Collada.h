// Copyright 2008-2014 Simon Ekström

#ifndef __BUILDER_COLLADA_H__
#define __BUILDER_COLLADA_H__

#include "Geometry.h"

namespace tinyxml2
{
	class XMLElement;
};

namespace sb
{

	namespace collada
	{
		struct Source
		{
			Source() : stride(0) {}

			string id;
			vector<float> data;
			uint32_t stride; // Number of floats per vertex
		};

		struct InputChannel
		{
			InputChannel() : offset(0), set(0) {}

			string semantic;
			string source;
			uint32_t offset; // Offset in number of floats
			uint32_t set; // Set index, mainly for texcoords
		};

		struct TriangleSet
		{
			TriangleSet() : count(0) {}

			uint32_t count; // number of triangles
			string material_id;

			vector<uint32_t> indices;
			vector<InputChannel> inputs;
		};

		struct Geometry
		{
			struct Vertices
			{
				Vertices()  {}

				string id;
				vector<InputChannel> inputs;
			};

			Geometry() {}


			vector<Source> sources;
			vector<TriangleSet> triangle_sets;

			Vertices vertices;

		};


		class ColladaFile
		{
		public:
			ColladaFile();
			~ColladaFile();

			/// @brief Parses a collada document
			/// @return True if parsing was successful, false if failed
			bool Parse(const char* data, uint32_t len);

			/// Converts the collada file into a mesh object
			/// @param mesh Target object
			void Convert(geometry::Mesh& mesh);


		private:
			const ColladaFile& operator=(const ColladaFile&) { return *this; }


			bool ParseGeometryLibrary(tinyxml2::XMLElement* element);
			bool ParseGeometry(tinyxml2::XMLElement* element);

			bool ParseInputs(tinyxml2::XMLElement* triangles_elem, vector<InputChannel>& inputs);

			vector<Geometry> _geometry;

			map<StringId32, string> _materials; ///< Material id to material name mapping
		};
	};

} // namespace sb


#endif // __BUILDER_COLLADA_H__
