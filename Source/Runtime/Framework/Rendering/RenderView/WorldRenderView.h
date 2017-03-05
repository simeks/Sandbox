// Copyright 2008-2014 Simon Ekström

#ifndef __FRAMEWORK_WORLDRENDERVIEW_H__
#define __FRAMEWORK_WORLDRENDERVIEW_H__

#include "RenderView.h"


namespace sb
{
	class ConfigValue;
	class WorldRenderView : public RenderView
	{
	public:
		WorldRenderView();
		~WorldRenderView();

		void Load(const ConfigValue& config,
				  RenderResourceAllocator* resource_allocator,
				  RenderResourceSet* resource_set) OVERRIDE;
		void Render(uint64_t sort_key, 
					const Params& params, 
					RenderContext* render_context) OVERRIDE;

	private:
		vector<pair<StringId32, StringId32>> _resources;

	};

} // namespace sb


#endif // __FRAMEWORK_WORLDRENDERVIEW_H__
