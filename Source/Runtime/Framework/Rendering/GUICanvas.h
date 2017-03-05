// Copyright 2008-2014 Simon Ekström

#ifndef __FRAMEWORK_GUICANVAS_H__
#define __FRAMEWORK_GUICANVAS_H__

#include <Foundation/Math/Vec2.h>
#include <Foundation/Math/Vec3.h>

#include <Engine/Rendering/RenderBlock.h>

namespace sb
{

struct GUIRect
{
	Vec3f pos;
	float width;
	float height;
	uint32_t color;

	GUIRect() : width(0.0f), height(0.0f) {}
};

struct GUITexturedRect : public GUIRect
{
	StringId32 material;
	Vec2f uv0; // UV for top left
	Vec2f uv1; // UV for bottom right

	GUITexturedRect()
		: uv0(0.0f, 0.0f), uv1(1.0f, 1.0f)
	{
	}
};

struct GUIText : public GUIRect
{
	StringId32 material;
	StringId64 font;
	float font_size;
	const char* text;

	GUIText()
		: font_size(0.0f)
	{
	}
};

struct Glyph;
struct Font;

class Material;
class MaterialManager;
class ResourceManager;
class RenderContext;
class RenderResourceAllocator;

class GUICanvas
{
public:
	GUICanvas(MaterialManager* material_manager,
			  ResourceManager* resource_manager,
			  RenderResourceAllocator* resource_allocator);
	~GUICanvas();
	
	void DrawTexturedRect(const GUITexturedRect& rect);
	void DrawText(const GUIText& rect);
	void DrawLine(uint32_t num_vertices, float* vertices, uint32_t color);

	void Flush(RenderContext* render_context, 
			   RenderResourceAllocator* resource_allocator, 
			   uint64_t sort_key);

private:
	enum { BATCH_INITIAL_VERTEX_COUNT = 256 };

	struct Vertex
	{
		Vec3f pos;
		Vec2f uv;
		uint32_t color;
	};

	struct Batch
	{
		RVertexBuffer vertex_buffer;
		vector<uint8_t> vertex_data;
		RenderBlock render_block;
		Material* material;
		uint32_t offset;

		Batch() {}
	};

	/// @remark This will not allocate the render resources for the batch.
	Batch* GetBatch(Material* material);

	void DrawRect(Batch* batch, const Vec3f& pos, float width, float height,
				  const Vec2f& uv0, const Vec2f& uv1, uint32_t color);


	RenderResourceAllocator* _resource_allocator;
	MaterialManager* _material_manager;
	ResourceManager* _resource_manager;

	RVertexDeclaration* _vertex_decl; ///< Vertex declaration for the textured rectangles

	map<Material*, Batch*> _batches;

};


}; // namespace sb

#endif // __FRAMEWORK_GUICANVAS_H__
