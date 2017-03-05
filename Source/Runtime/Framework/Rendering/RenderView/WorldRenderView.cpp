// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "WorldRenderView.h"
#include "World/RenderWorld.h"
#include "World/RenderComponent.h"
#include "World/MeshComponent.h"

#include "Rendering/Layer.h"
#include "Rendering/RenderResourceSet.h"

#include <Foundation/Container/ConfigValue.h>

#include <Engine/Rendering/ShaderParameters.h>

namespace sb
{

	WorldRenderView::WorldRenderView()
	{

	}
	WorldRenderView::~WorldRenderView()
	{

	}

	void WorldRenderView::Load(const ConfigValue& config,
							   RenderResourceAllocator* ,
							   RenderResourceSet* )
	{
		const ConfigValue& resources = config["resources"];
		if (resources.IsArray())
		{
			for (uint32_t i = 0; i < resources.Size(); ++i)
			{
				const ConfigValue& res_cfg = resources[i];
				Assert(res_cfg.IsArray() && (res_cfg.Size() == 2));

				_resources.push_back(pair<StringId32, StringId32>(res_cfg[0].AsString(),
																  res_cfg[1].AsString()));
			}
		}

	}

	void WorldRenderView::Render(uint64_t sort_key, const Params& params, RenderContext* render_context)
	{
		for (auto& resource : _resources)
		{
			RenderResource* r = params.resources->GetResource(resource.second);
			if (r)
			{
				params.shader_params->SetResource(resource.first, *r);
			}
		}

		params.layer->Bind(render_context, params.resources, *params.viewport);

		for (auto& object : params.world->GetObjects()) // TODO: Culling
		{
			RenderComponent::Type object_type = object->GetRenderType();

			switch (object_type)
			{
			case RenderComponent::MESH:
				((MeshComponent*)object)->Render(render_context, params.shader_params, params.camera, params.layer, sort_key);

			default:
				break;
			};
		}

	}

} // namespace sb


