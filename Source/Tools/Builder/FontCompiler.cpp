// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "FontCompiler.h"
#include "DependencyDatabase.h"
#include "stblib/stb_image.h"

#include <Framework/Rendering/Font.h>

#include <Foundation/IO/InputBuffer.h>
#include <Foundation/IO/FileInputBuffer.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/tinyxml2/tinyxml2.h>

namespace sb
{

FontCompiler::FontCompiler(const ConfigValue& config)
	: CompilerSystem::Compiler(config)
{
}
FontCompiler::~FontCompiler()
{
}

CompilerSystem::Result FontCompiler::Compile(const FilePath& source_file, 
											 const FilePath& target_file, 
											 const CompilerSystem::CompilerContext& context)
{
	FileStreamPtr file = context.asset_source->OpenFile(source_file.c_str(), File::READ);
	if (!file.Get() || file->Length() < 0)
	{
		SetError("Could not open file.");
		return CompilerSystem::FAILED;
	}

	FileInputBuffer buffer(file);
	file.Reset();

	tinyxml2::XMLDocument doc;
	doc.Parse((const char*)buffer.Ptr(), (size_t)buffer.Length());

	tinyxml2::XMLElement* root = doc.RootElement();
	if (strcmp(root->Name(), "font") != 0)
	{
		SetError("Invalid font file.");
		return CompilerSystem::FAILED;
	}

	tinyxml2::XMLElement* info = root->FirstChildElement("info");
	tinyxml2::XMLElement* common = root->FirstChildElement("common");
	tinyxml2::XMLElement* pages = root->FirstChildElement("pages");
	tinyxml2::XMLElement* chars = root->FirstChildElement("chars");

	if (!info || !common || !pages || !chars)
	{
		SetError("Invalid font file.");
		return CompilerSystem::FAILED;
	}

	Font font;
	tinyxml2::XMLUtil::ToFloat(info->Attribute("size"), &font.data.size);
	tinyxml2::XMLUtil::ToFloat(common->Attribute("lineHeight"), &font.data.line_height);
	tinyxml2::XMLUtil::ToFloat(common->Attribute("base"), &font.data.base);

	tinyxml2::XMLUtil::ToUnsigned(chars->Attribute("count"), &font.data.glyph_count);
	font.data.glyphs = new Glyph[font.data.glyph_count];

	tinyxml2::XMLElement* char_elem = chars->FirstChildElement("char");
	for (uint32_t i = 0; i < font.data.glyph_count; ++i)
	{
		Glyph& glyph = font.data.glyphs[i];

		tinyxml2::XMLUtil::ToUnsigned(char_elem->Attribute("id"), &glyph.code);
		tinyxml2::XMLUtil::ToFloat(char_elem->Attribute("x"), &glyph.x);
		tinyxml2::XMLUtil::ToFloat(char_elem->Attribute("y"), &glyph.y);
		tinyxml2::XMLUtil::ToFloat(char_elem->Attribute("width"), &glyph.width);
		tinyxml2::XMLUtil::ToFloat(char_elem->Attribute("height"), &glyph.height);
		tinyxml2::XMLUtil::ToFloat(char_elem->Attribute("xoffset"), &glyph.xoffset);
		tinyxml2::XMLUtil::ToFloat(char_elem->Attribute("yoffset"), &glyph.yoffset);
		tinyxml2::XMLUtil::ToFloat(char_elem->Attribute("xadvance"), &glyph.xadvance);

		char_elem = char_elem->NextSiblingElement("char");
	}

	tinyxml2::XMLElement* page = pages->FirstChildElement("page");
	if (!page)
	{
		SetError("Invalid font file.");
		return CompilerSystem::FAILED;
	}

	FilePath texture_path;
	texture_path = source_file.Directory();
	texture_path += page->Attribute("file");
	texture_path.SetSeparator('/');
	context.dependency_database->AddDependent(texture_path.c_str(), source_file.c_str());

	texture_path = context.asset_source->GetFullPath(); // Get the full path
	texture_path += "/";
	texture_path += source_file.Directory();
	texture_path += page->Attribute("file");
	texture_path.SetSeparator('/');

	int x,y,n;
	unsigned char *texture_data = stbi_load(texture_path.c_str(), &x, &y, &n, 1);
	if (!texture_data)
	{
		SetError("Failed to load font texture.");
		return CompilerSystem::FAILED;
	}

	font.data.texture_width = x;
	font.data.texture_height = y;

	vector<uint8_t> data;
	DynamicMemoryStream stream(&data);
	font_resource::Compile(&font.data, texture_data, stream);
	if (!WriteAsset(context.asset_target, target_file, data.data(), (uint32_t) data.size()))
		return CompilerSystem::FAILED;

	stbi_image_free(texture_data);
	delete[] font.data.glyphs;

	return CompilerSystem::SUCCESSFUL;
}


} // namespace sb

