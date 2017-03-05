// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "RenderViewSet.h"
#include "RenderView.h"
#include "WorldRenderView.h"
#include "ShadowMappingView.h"

#include <Foundation/Container/ConfigValue.h>



namespace sb
{

	RenderViewSet::RenderViewSet()
	{
	}
	RenderViewSet::~RenderViewSet()
	{
	}

	void RenderViewSet::Load(	const ConfigValue& config, 
								RenderResourceAllocator* resource_allocator, 
								RenderResourceSet* resource_set)
	{
		Assert(config.IsArray());
		for (uint32_t i = 0; i < config.Size(); ++i)
		{
			const ConfigValue& view_cfg = config[i];
			Assert(view_cfg["name"].IsString());
			StringId32 name = view_cfg["name"].AsString();

			map<StringId32, RenderView*>::iterator it = _views.find(name);
			if (it != _views.end())
			{
				logging::Warning("RenderViewSet: View with the name '%s' already loaded", view_cfg["name"].AsString());
				continue;
			}

			Assert(view_cfg["type"].IsString());
			const char* view_type = view_cfg["type"].AsString();
			if (strcmp(view_type, "world_render_view") == 0)
			{
				WorldRenderView* render_view = new WorldRenderView();
				render_view->Load(view_cfg, resource_allocator, resource_set);
				_views[name] = render_view;
			}
			else if (strcmp(view_type, "shadow_mapping_view") == 0)
			{
				ShadowMappingView* render_view = new ShadowMappingView();
				render_view->Load(view_cfg, resource_allocator, resource_set);
				_views[name] = render_view;
			}
			else
			{
				AssertMsg(false, "Unrecognized render view type");
			}

		}
	}
	void RenderViewSet::Unload(RenderResourceAllocator* resource_allocator)
	{
		for (auto& view : _views)
		{
			view.second->Unload(resource_allocator);
			delete view.second;
		}
		_views.clear();
	}

	RenderView* RenderViewSet::Get(StringId32 name)
	{
		map<StringId32, RenderView*>::iterator it = _views.find(name);
		if (it != _views.end())
			return it->second;
		return nullptr;
	}

} // namespace sb
