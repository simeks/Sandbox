// Copyright 2008-2014 Simon Ekström

#ifndef __FRAMEWORK_FONT_H__
#define __FRAMEWORK_FONT_H__

#include <Foundation/Resource/ResourceManager.h>
#include <Engine/Rendering/RTexture.h>

namespace sb
{

class RenderResourceAllocator;

struct Glyph
{
	uint32_t code;
	float x;
	float y;
	float width;
	float height;
	float xoffset;
	float yoffset;
	float xadvance;
};

struct FontData
{
	float size;
	float line_height;
	float base;

	uint32_t glyph_count;
	Glyph* glyphs;

	uint32_t texture_width;
	uint32_t texture_height;
};

struct Font
{
	FontData data;
	RTexture texture;

	/// @return Glyph with the specified code if glyph was found, NULL if not.
	Glyph* GetGlyph(uint32_t code);
};

namespace font_resource
{
enum { FONT_RESOURCE_VERSION = 1 };

void Compile(FontData* font_data, uint8_t* texture_data, Stream& stream);

void Load(ResourceLoader::LoadContext& context);
void Unload(ResourceLoader::UnloadContext& context);

void RegisterResourceType(ResourceManager* resource_manager,
						  RenderResourceAllocator* resource_allocator);
void UnregisterResourceType(ResourceManager* resource_manager);

};

} // namespace sb

#endif // __FRAMEWORK_FONT_H__
