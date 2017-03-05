// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Font.h"

#include <Foundation/IO/Stream.h>
#include <Engine/Rendering/Texture.h>
#include <Engine/Rendering/RenderResourceAllocator.h>


namespace sb
{

Glyph* Font::GetGlyph(uint32_t code)
{
	for (uint32_t i = 0; i < data.glyph_count; ++i)
	{
		if (data.glyphs[i].code == code)
			return &data.glyphs[i];
	}
	return nullptr;
}

void font_resource::Compile(FontData* font_data, uint8_t* texture_data, Stream& stream)
{
	uint32_t version = FONT_RESOURCE_VERSION;
	stream.Write(&version, 4);

	stream.Write(&font_data->size, sizeof(float));
	stream.Write(&font_data->line_height, sizeof(float));
	stream.Write(&font_data->base, sizeof(float));
	stream.Write(&font_data->glyph_count, sizeof(uint32_t));
	stream.Write(font_data->glyphs, sizeof(*font_data->glyphs)*font_data->glyph_count);
	stream.Write(&font_data->texture_width, sizeof(uint32_t));
	stream.Write(&font_data->texture_height, sizeof(uint32_t));
	stream.Write(texture_data, font_data->texture_width*font_data->texture_height);
}

void font_resource::Load(ResourceLoader::LoadContext& context)
{
	uint32_t version;

	// Shader version
	context.file->Read(&version, 4);
	if (version != FONT_RESOURCE_VERSION)
	{
		logging::Error("Failed loading font: Wrong version, tried loading version %d, current version is %d.", version, FONT_RESOURCE_VERSION);
	}

	Font* font = new Font;
	context.file->Read(&font->data.size, sizeof(float));
	context.file->Read(&font->data.line_height, sizeof(float));
	context.file->Read(&font->data.base, sizeof(float));
	context.file->Read(&font->data.glyph_count, sizeof(uint32_t));

	font->data.glyphs = new Glyph[font->data.glyph_count];
	context.file->Read(font->data.glyphs, sizeof(*font->data.glyphs)*font->data.glyph_count);
	context.file->Read(&font->data.texture_width, sizeof(uint32_t));
	context.file->Read(&font->data.texture_height, sizeof(uint32_t));

	uint32_t texture_size = font->data.texture_width*font->data.texture_height;
	uint8_t* texture_data = new uint8_t[texture_size];
	context.file->Read(texture_data, texture_size);

	TextureDesc texture_desc;
	texture_desc.pixel_format = image::PF_R8_UNORM;
	texture_desc.width = font->data.texture_width;
	texture_desc.height = font->data.texture_height;
	texture_desc.mip_count = 1;
	texture_desc.array_size = 1;
	texture_desc.type = TextureDesc::TYPE_2D;
	texture_desc.usage = TextureDesc::STATIC;
	
	font->texture = RTexture(texture_desc);

	RenderResourceAllocator* resource_allocator = (RenderResourceAllocator*)context.user_data;

	vector<image::Surface> surfaces;
	surfaces.push_back(image::Surface());
	surfaces.back().data = texture_data;
	surfaces.back().size = texture_size;

	resource_allocator->AllocateTexture(font->texture, &surfaces);

	delete[] texture_data;

	context.result = font;
}
void font_resource::Unload(ResourceLoader::UnloadContext& context)
{
	Font* font = (Font*)context.resource_data;

	RenderResourceAllocator* resource_allocator = (RenderResourceAllocator*) context.user_data;
	resource_allocator->ReleaseResource(font->texture);

	delete [] font->data.glyphs;
	delete font;
}

void font_resource::RegisterResourceType(ResourceManager* resource_manager, 
										 RenderResourceAllocator* resource_allocator)
{
	ResourceType resource_type;
	resource_type.user_data = nullptr;
	resource_type.load_callback = Load;
	resource_type.unload_callback = Unload;
	resource_type.user_data = resource_allocator;

	resource_manager->RegisterType("font", resource_type);
}
void font_resource::UnregisterResourceType(ResourceManager* resource_manager)
{
	resource_manager->UnregisterType("font");
}


} // namespace sb

