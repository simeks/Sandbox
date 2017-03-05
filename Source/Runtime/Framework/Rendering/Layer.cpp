// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Layer.h"
#include "RenderResourceSet.h"

#include <Engine/Rendering/RenderContext.h>
#include <Engine/Rendering/RRenderTarget.h>


namespace sb
{

	void Layer::Bind(RenderContext* context, RenderResourceSet* global_resources, const Viewport& viewport) const
	{
		context->ClearState(sort_key);

		RenderContext::Targets targets;

		for (uint32_t i = 0; i < MAX_MULTIPLE_RENDER_TARGETS; ++i)
		{
			if (render_targets[i] != "")
			{
				targets.render_targets[i] = *global_resources->GetResource(render_targets[i]);
			}
			else
			{
				targets.render_targets[i] = RRenderTarget();
			}
		}
		if (depth_stencil_target != "")
		{
			targets.depth_stencil_target = *global_resources->GetResource(depth_stencil_target);
		}
		else
		{
			targets.depth_stencil_target = RRenderTarget();
		}

		targets.num_viewports = 1;
		targets.viewports = &viewport;

		targets.num_scissor_rects = 0;
		targets.rects = nullptr;

		context->SetTargets(sort_key, targets);

		if (clear_flags != 0)
		{
			float clear_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			context->ClearTargets(sort_key, clear_flags, clear_color);
		}
	}

} // namespace sb

