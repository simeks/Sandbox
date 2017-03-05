// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_LAYER_H__
#define __RENDERING_LAYER_H__

#include <Engine/Rendering/Rendering.h>

namespace sb
{

	class RenderContext;
	class RenderResourceSet;

	struct Layer
	{
		StringId32 name;
		uint64_t sort_key;

		vector<StringId32> render_views;
		StringId32 technique; // Shader technique

		StringId32 render_targets[MAX_MULTIPLE_RENDER_TARGETS];
		StringId32 depth_stencil_target;
		uint8_t clear_flags;
		render_sorting::DepthSort depth_sort;

		Layer()
		{
			name = "";
			sort_key = 0;
			depth_stencil_target = "";
			for (uint32_t i = 0; i < MAX_MULTIPLE_RENDER_TARGETS; ++i)
				render_targets[i] = "";

			clear_flags = 0;
			depth_sort = render_sorting::FRONT_TO_BACK;
		}

		/// Dispatches a render state command binding any needed resources to the pipeline.
		void Bind(RenderContext* context, RenderResourceSet* global_resources, const Viewport& viewport) const;
	};

} // namespace sb



#endif // __RENDERING_LAYER_H__
