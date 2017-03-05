// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_RVERTEXDECLARATION_H__
#define __RENDERING_RVERTEXDECLARATION_H__

#include "RenderResource.h"

namespace sb
{
	class Stream;
	class RVertexDeclaration : public RenderResource
	{
	public:
		enum { MAX_ELEMENT_COUNT = 16 };

		enum ElementSemantic
		{
			ES_POSITION,
			ES_NORMAL,
			ES_TANGENT,
			ES_BINORMAL,
			ES_TEXCOORD,
			ES_COLOR
		};
		enum ElementType
		{
			ET_FLOAT1,
			ET_FLOAT2,
			ET_FLOAT3,
			ET_FLOAT4,
			ET_UBYTE4
		};

		struct Element
		{
			ElementSemantic semantic;
			uint32_t sem_index; // Semantic index
			ElementType type;
			uint32_t slot; // Vertex stream input slot
		};

		RVertexDeclaration();

		void Clear();

		void AddElement(const Element& element);

		const Element& GetElement(uint32_t i) const;
		uint32_t VertexSize() const;
		uint32_t Count() const;

		void Serialize(Stream& stream) const;
		void Deserialize(Stream& stream);

	private:
		Element _elements[MAX_ELEMENT_COUNT];
		uint32_t _count;

	};

} // namespace sb



#endif // __RENDERING_RVERTEXDECLARATION_H__

