// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "DebugGUI.h"

#include <Framework/World/World.h>
#include <Framework/Rendering/RenderResourceSet.h>
#include <Framework/Rendering/GUICanvas.h>
#include <Engine/Rendering/MaterialManager.h>
#include <Engine/Rendering/Material.h>
#include <Engine/Rendering/RTexture.h>
#include <Engine/Rendering/RRenderTarget.h>

namespace sb
{

DebugGUI::DebugGUI(MaterialManager* material_manager,
				   ResourceManager* resource_manager,
				   RenderResourceSet* resources,
				   RenderResourceAllocator* resource_allocator,
				   uint32_t width,
				   uint32_t height)
	: _width(width),
	  _height(height),
	  _draw_shadow_maps(false)
{
	_canvas = new GUICanvas(material_manager, resource_manager, resource_allocator);

	InitShadowMapView(material_manager, resources);
}
DebugGUI::~DebugGUI()
{
	delete _canvas;
}

void DebugGUI::Draw()
{
	if (_draw_shadow_maps)
	{
		GUITexturedRect trect;
		trect.pos = Vec3f(0.0f, _height - 250.0f, 0.0f);
		trect.height = 250.0f;
		trect.width = 400.0f;
		trect.color = 0xFFFFFFFF;
		trect.material = "core/debug_gui/shadow_map0";

		_canvas->DrawTexturedRect(trect);

		trect.pos = Vec3f(400.0f, _height - 250.0f, 0.0f);
		trect.height = 250.0f;
		trect.width = 400.0f;
		trect.color = 0xFFFFFFFF;
		trect.material = "core/debug_gui/shadow_map1";

		_canvas->DrawTexturedRect(trect);

		trect.pos = Vec3f(800.0f, _height - 250.0f, 0.0f);
		trect.height = 250.0f;
		trect.width = 400.0f;
		trect.color = 0xFFFFFFFF;
		trect.material = "core/debug_gui/shadow_map2";

		_canvas->DrawTexturedRect(trect);
	}

}

GUICanvas* DebugGUI::GetCanvas()
{
	return _canvas;
}

void DebugGUI::DrawText(int nX, int nY, uint32_t nColor, const char* pText, uint32_t )
{
	GUIText text;
	text.pos = Vec3f((float) nX, (float) nY, 0.0f);
	text.color = nColor;
	text.text = pText;
	text.font_size = 16.0f;
	text.font = "core/fonts/consolas_small";
	text.material = "core/fonts/text_material";
	_canvas->DrawText(text);
}
void DebugGUI::DrawBox(int nX, int nY, int nX1, int nY1, uint32_t nColor)
{
	GUITexturedRect trect;
	trect.pos = Vec3f((float) nX, (float) nY, 0.0f);
	trect.width = float(nX1 - nX);
	trect.height = float(nY1 - nY);
	trect.color = nColor;
	trect.material = "core/gui/blank";

	_canvas->DrawTexturedRect(trect);
}
void DebugGUI::DrawLine2D(uint32_t nVertices, float* pVertices, uint32_t nColor)
{
	_canvas->DrawLine(nVertices, pVertices, nColor);
}

void DebugGUI::InitShadowMapView(MaterialManager* material_manager, RenderResourceSet* resources)
{
	Material* shadow_map_material_0 = (Material*)material_manager->GetMaterial("core/debug_gui/shadow_map0");
	Assert(shadow_map_material_0);
	Material* shadow_map_material_1 = (Material*)material_manager->GetMaterial("core/debug_gui/shadow_map1");
	Assert(shadow_map_material_1);
	Material* shadow_map_material_2 = (Material*)material_manager->GetMaterial("core/debug_gui/shadow_map2");
	Assert(shadow_map_material_2);

	RTexture* shadow_map = (RTexture*)resources->GetResource("cascaded_shadow_map");

	shadow_map_material_0->SetTexture("diffuse_map", *shadow_map);
	shadow_map_material_1->SetTexture("diffuse_map", *shadow_map);
	shadow_map_material_2->SetTexture("diffuse_map", *shadow_map);
}

}; // namespace sb
