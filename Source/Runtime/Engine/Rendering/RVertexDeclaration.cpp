// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "RVertexDeclaration.h"

#include <Foundation/IO/Stream.h>

namespace sb
{

	namespace
	{
		static uint32_t element_size[] = {
			/* ET_FLOAT1 */ 4,
			/* ET_FLOAT2 */ 8,
			/* ET_FLOAT3 */ 12,
			/* ET_FLOAT4 */ 16,
			/* ET_UBYTE4 */ 4
		};
	}

	RVertexDeclaration::RVertexDeclaration() : RenderResource(VERTEX_DECLARATION), _count(0)
	{
	}

	void RVertexDeclaration::Clear()
	{
		_count = 0;
	}

	const RVertexDeclaration::Element& RVertexDeclaration::GetElement(uint32_t i) const
	{
		return _elements[i];
	}
	void RVertexDeclaration::AddElement(const Element& element)
	{
		Assert(_count < MAX_ELEMENT_COUNT);
		_elements[_count++] = element;
	}

	uint32_t RVertexDeclaration::VertexSize() const
	{
		uint32_t size = 0;
		for (uint32_t e = 0; e < _count; ++e)
		{
			size += element_size[_elements[e].type];
		}
		return size;
	}
	uint32_t RVertexDeclaration::Count() const
	{
		return _count;
	}

	void RVertexDeclaration::Serialize(Stream& stream) const
	{
		stream.Write(&_count, 4);
		stream.Write(_elements, sizeof(Element)*_count);
	}
	void RVertexDeclaration::Deserialize(Stream& stream)
	{
		stream.Read(&_count, 4);
		stream.Read(_elements, sizeof(Element)*_count);
	}

} // namespace sb

