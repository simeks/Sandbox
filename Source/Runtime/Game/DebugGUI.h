// Copyright 2008-2014 Simon Ekström

#ifndef __GAME_DEBUGGUI_H__
#define __GAME_DEBUGGUI_H__

#include <Foundation/Profiler/Profiler.h>

namespace sb 
{
class GUICanvas;
class World;
class MaterialManager;
class RenderResourceSet;
class RenderResourceAllocator;
class ResourceManager;

class DebugGUI : public profiler::DrawCallback
{
public:
	DebugGUI(MaterialManager* material_manager,
			 ResourceManager* resource_manager,
			 RenderResourceSet* resources,
			 RenderResourceAllocator* resource_allocator,
			 uint32_t width,
			 uint32_t height);
	~DebugGUI();

	void Draw();

	GUICanvas* GetCanvas();

	/* profiler::DrawCallback */

	virtual void DrawText(int nX, int nY, uint32_t nColor, const char* pText, 
						  uint32_t nNumCharacters) OVERRIDE;
	virtual void DrawBox(int nX, int nY, int nX1, int nY1, uint32_t nColor) OVERRIDE;
	virtual void DrawLine2D(uint32_t nVertices, float* pVertices, uint32_t nColor) OVERRIDE;

private:
	void InitShadowMapView(MaterialManager* material_manager, RenderResourceSet* resources);

	GUICanvas* _canvas;

	uint32_t _width;
	uint32_t _height;

	bool _draw_shadow_maps;
};

}; // namespace sb

#endif // __GAME_DEBUGGUI_H__
