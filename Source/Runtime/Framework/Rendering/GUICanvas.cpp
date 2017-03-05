// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "GUICanvas.h"
#include "Rendering/Font.h"

#include <Engine/Rendering/RenderResourceAllocator.h>
#include <Engine/Rendering/RenderContext.h>
#include <Engine/Rendering/RVertexBuffer.h>
#include <Engine/Rendering/RVertexDeclaration.h>
#include <Engine/Rendering/ShaderManager.h>
#include <Engine/Rendering/MaterialManager.h>
#include <Engine/Rendering/RShader.h>
#include <Engine/Rendering/Material.h>

#include <Foundation/Resource/ResourceManager.h>

namespace sb
{

GUICanvas::GUICanvas(MaterialManager* material_manager,
					 ResourceManager* resource_manager,
					 RenderResourceAllocator* resource_allocator)
	: _resource_allocator(resource_allocator),
	  _material_manager(material_manager),
	  _resource_manager(resource_manager),
	  _vertex_decl(nullptr)
{
	// Create the shared vertex declaration
	_vertex_decl = new RVertexDeclaration();
	{
		RVertexDeclaration::Element elem;
		elem.semantic = RVertexDeclaration::ES_POSITION;
		elem.sem_index = 0;
		elem.slot = 0;
		elem.type = RVertexDeclaration::ET_FLOAT3;
		_vertex_decl->AddElement(elem);

		elem.semantic = RVertexDeclaration::ES_TEXCOORD;
		elem.type = RVertexDeclaration::ET_FLOAT2;
		_vertex_decl->AddElement(elem);

		elem.semantic = RVertexDeclaration::ES_COLOR;
		elem.type = RVertexDeclaration::ET_UBYTE4;  
		_vertex_decl->AddElement(elem);
	}
	_resource_allocator->AllocateVertexDeclaration(*_vertex_decl);
}
GUICanvas::~GUICanvas()
{
	for (auto& batch : _batches)
	{
		_resource_allocator->ReleaseResource(batch.second->vertex_buffer);
		delete batch.second;
	}
	_batches.clear();


	_resource_allocator->ReleaseResource(*_vertex_decl);

	delete _vertex_decl;
	_vertex_decl = nullptr;
}

void GUICanvas::DrawTexturedRect(const GUITexturedRect& rect)
{
	Material* material = _material_manager->GetMaterial(rect.material);
	Assert(material);

	Batch* batch = GetBatch(material);
	DrawRect(batch, rect.pos, rect.width, rect.height, rect.uv0, rect.uv1, rect.color);
}

void GUICanvas::DrawText(const GUIText& rect)
{
	bool has_shadow = true;

	Material* material = _material_manager->GetMaterial(rect.material);
	Assert(material);

	Font* font = (Font*) _resource_manager->GetResource("font", rect.font);
	material->SetTexture("diffuse_map", font->texture); // TODO: Cloning materials

	Batch* batch = GetBatch(material);

	float tex_scale_w = 1.0f / font->data.texture_width;
	float tex_scale_h = 1.0f / font->data.texture_height;
	float font_scale = rect.font_size != 0.0f ? (rect.font_size / font->data.size) : 1.0f;

	Vec3f position = rect.pos;
	for (const char* c = rect.text; *c != '\0'; ++c)
	{
		if (*c == '\n')
		{
			position.x = rect.pos.x;
			position.y = position.y + font->data.line_height;
			continue;
		}

		Glyph* glyph = font->GetGlyph(*c);
		if (!glyph)
			continue;

		if (rect.width != 0.0f && (position.x + glyph->width > rect.pos.x + rect.width))
		{
			position.x = rect.pos.x;
			position.y = position.y + font->data.line_height;
		}

		if (rect.height != 0.0f && (position.y > rect.pos.y + rect.height))
		{
			break;
		}

		Vec3f glyph_position = position + Vec3f(glyph->xoffset, glyph->yoffset, 0.0f)*font_scale;

		float width = glyph->width*font_scale;
		float height = glyph->height*font_scale;
		Vec2f uv0 = Vec2f(glyph->x*tex_scale_w, glyph->y*tex_scale_h);
		Vec2f uv1 = Vec2f((glyph->x + glyph->width)*tex_scale_w, (glyph->y + glyph->height)*tex_scale_h);

		if (has_shadow)
		{
			DrawRect(batch, glyph_position + Vec3f(1.0f, 1.0f, 0.0f), 
					 width, height, uv0, uv1, 
					 0xFF000000); // Shadow color should be black
		}

		DrawRect(batch, glyph_position,
				 width, height, uv0, uv1,
				 rect.color); // Shadow color should be black

		position.x += glyph->xadvance*font_scale;

	}

}
void GUICanvas::DrawLine(uint32_t num_vertices, float* vertices, uint32_t color)
{
	Material* material = _material_manager->GetMaterial("core/gui/line_drawer");
	Assert(material);

	Batch* batch = GetBatch(material);
	batch->render_block.draw_call.prim_type = DrawCall::PRIMITIVE_LINE_LIST;

	if (batch->offset + 2 * num_vertices * _vertex_decl->VertexSize() > batch->vertex_data.size())
	{
		batch->vertex_data.resize(batch->vertex_data.size() + 2 * num_vertices * _vertex_decl->VertexSize());
	}

	Vertex* vertex = (Vertex*) memory::PointerAdd(batch->vertex_data.data(), batch->offset);
	batch->offset += num_vertices * _vertex_decl->VertexSize();

	for (uint32_t i = 0; i < num_vertices-1; ++i)
	{
		vertex->pos = Vec3f(vertices[2*i], vertices[2*i+1], 0.0f);
		vertex->color = color;
		++vertex;

		vertex->pos = Vec3f(vertices[2*i+2], vertices[2*i+3], 0.0f);
		vertex->color = color;
		++vertex;
	}
}


void GUICanvas::Flush(RenderContext* render_context,
					  RenderResourceAllocator* resource_allocator,
					  uint64_t sort_key)
{
	uint64_t key_user_data = 1; // User-data for the sort key

	for (auto& batch_entry : _batches)
	{
		Batch* batch = batch_entry.second;

		bool resize = false;

		// Check if buffer needs to be resized
		uint32_t vertex_count = (uint32_t)batch->vertex_data.size() / _vertex_decl->VertexSize();
		if (vertex_count > batch->vertex_buffer.GetDesc().vertex_count)
		{
			batch->vertex_buffer.GetDesc().vertex_count = Max(batch->vertex_buffer.GetDesc().vertex_count*2, 
															  vertex_count);
			
			batch->vertex_data.resize(batch->vertex_buffer.GetDesc().vertex_count * _vertex_decl->VertexSize());
			resize = true;
		}

		// Check if we need to allocate the vertex buffer for this batch
		if(IsInvalid(batch->render_block.vertex_buffer.GetHandle()))
		{
			resource_allocator->AllocateVertexBuffer(batch->vertex_buffer, batch->vertex_data.data());
			batch->render_block.vertex_buffer = batch->vertex_buffer;
		}
		else
		{
			if (resize)
			{
				resource_allocator->ReleaseResource(batch->vertex_buffer);
				resource_allocator->AllocateVertexBuffer(batch->vertex_buffer, batch->vertex_data.data());
				batch->render_block.vertex_buffer = batch->vertex_buffer;
			}
			else
			{
				render_context->UpdateBuffer(sort_key, batch->vertex_buffer, batch->vertex_data.data());
			}
		}
		batch->render_block.draw_call.vertex_count = vertex_count;


		Material* material = batch_entry.first;
		Shader* shader = material->GetShader();
		ShaderContext* shader_context = material->GetShaderContext();
		ShaderResourceBinder& resource_binder = shader->GetShaderResourceBinder();

		// Bind material parameters
		if (material)
		{
			resource_binder.Bind(shader_context->resources, material->GetShaderParams());
		}

		render_context->Draw((sort_key | (key_user_data << render_sorting::USER_DATA_BIT)), batch->render_block, *shader_context);

		// Clear batches after they have been rendered
		batch->vertex_data.clear();
		batch->offset = 0;
	}
}


GUICanvas::Batch* GUICanvas::GetBatch(Material* material)
{
	auto it = _batches.find(material);
	if(it != _batches.end()) // Batch for this material already exists
		return it->second;

	Batch* batch = new Batch();
	batch->material = material;

	batch->render_block.draw_call.index_count = 0;
	batch->render_block.draw_call.instance_count = 0;
	batch->render_block.draw_call.vertex_offset = 0;
	batch->render_block.draw_call.prim_type = DrawCall::PRIMITIVE_TRIANGLELIST;

	batch->render_block.vertex_declaration = *_vertex_decl;

	uint32_t vertex_size = _vertex_decl->VertexSize();
	uint32_t vertex_count = BATCH_INITIAL_VERTEX_COUNT;

	VertexBufferDesc vb_desc;
	vb_desc.vertex_size = _vertex_decl->VertexSize();
	vb_desc.vertex_count = BATCH_INITIAL_VERTEX_COUNT;
	vb_desc.usage = hardware_buffer::DYNAMIC;

	batch->vertex_buffer = RVertexBuffer(vb_desc);
	batch->render_block.vertex_buffer = batch->vertex_buffer;

	batch->vertex_data.resize(vertex_size * vertex_count);
	batch->offset = 0;

	_batches[material] = batch;

	return batch;
}

void GUICanvas::DrawRect(Batch* batch, const Vec3f& pos, float width, float height,
						 const Vec2f& uv0, const Vec2f& uv1, uint32_t color)
{
	Assert(batch);
	if (batch->offset + 6 * _vertex_decl->VertexSize() > batch->vertex_data.size())
	{
		batch->vertex_data.resize(batch->vertex_data.size() + 6 * _vertex_decl->VertexSize());
	}

	Vertex* vertex = (Vertex*) memory::PointerAdd(batch->vertex_data.data(), batch->offset);
	batch->offset += 6 * _vertex_decl->VertexSize();

	// Top-left
	vertex->pos = Vec3f(pos.x, pos.y, pos.z);
	vertex->uv = Vec2f(uv0.x, uv0.y);
	vertex->color = color;
	++vertex;

	// Bottom-left
	vertex->pos = Vec3f(pos.x, pos.y + height, pos.z);
	vertex->uv = Vec2f(uv0.x, uv1.y);
	vertex->color = color;
	++vertex;

	// Top-right
	vertex->pos = Vec3f(pos.x + width, pos.y, pos.z);
	vertex->uv = Vec2f(uv1.x, uv0.y);
	vertex->color = color;
	++vertex;


	// Bottom-left
	vertex->pos = Vec3f(pos.x, pos.y + height, pos.z);
	vertex->uv = Vec2f(uv0.x, uv1.y);
	vertex->color = color;
	++vertex;

	// Bottm-right
	vertex->pos = Vec3f(pos.x + width, pos.y + height, pos.z);
	vertex->uv = Vec2f(uv1.x, uv1.y);
	vertex->color = color;
	++vertex;

	// Top-right
	vertex->pos = Vec3f(pos.x + width, pos.y, pos.z);
	vertex->uv = Vec2f(uv1.x, uv0.y);
	vertex->color = color;
	++vertex;

}

} // namespace sb

