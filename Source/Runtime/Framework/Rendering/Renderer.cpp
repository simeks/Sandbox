// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Renderer.h"
#include "ShadingEnvironment.h"
#include "RenderView/RenderView.h"
#include "World/World.h"
#include "World/RenderWorld.h"
#include "World/Camera.h"
#include "Font.h"
#include "GUICanvas.h"

#include <Foundation/Container/ConfigValue.h>
#include <Foundation/Profiler/Profiler.h>

#include <Engine/Rendering/NullRenderDevice.h>
#include <Engine/Rendering/RRenderTarget.h>
#include <Engine/Rendering/RenderContext.h>
#include <Engine/Rendering/ShaderManager.h>
#include <Engine/Rendering/MaterialManager.h>
#include <Engine/Rendering/Texture.h>
#include <Engine/Rendering/Mesh.h>

#include <RenderD3D11/Common.h>
#include <RenderD3D11/D3D11RenderDevice.h>



namespace sb
{

//-------------------------------------------------------------------------------
Renderer::Renderer(ResourceManager* resource_manager, TaskScheduler* scheduler)
	: _resource_manager(resource_manager),
	_scheduler(scheduler),
	_device(nullptr),
	_shader_manager(nullptr),
	_material_manager(nullptr)
{
}
Renderer::~Renderer()
{
}
//-------------------------------------------------------------------------------
void Renderer::Initialize(const RenderDevice::InitParams& params)
{
	_device = new SANDBOX_RENDER_DEVICE();

	_device->Initialize(params);

	_shader_manager = new ShaderManager(_device, _resource_manager);
	_material_manager = new MaterialManager(_shader_manager, _resource_manager);

	texture_resource::RegisterResourceType(_resource_manager, _device);
	mesh_resource::RegisterResourceType(_resource_manager, _device);
	font_resource::RegisterResourceType(_resource_manager, _device->GetResourceAllocator());
}
//-------------------------------------------------------------------------------
void Renderer::Shutdown()
{
	font_resource::UnregisterResourceType(_resource_manager);
	mesh_resource::UnregisterResourceType(_resource_manager);
	texture_resource::UnregisterResourceType(_resource_manager);

	delete _material_manager;
	_material_manager = nullptr;

	delete _shader_manager;
	_shader_manager = nullptr;

	_device->Shutdown();

	delete _device;
	_device = nullptr;
}

//-------------------------------------------------------------------------------

void Renderer::LoadRenderSetup(const ConfigValue& render_setup)
{
	RenderResourceAllocator* resource_allocator = _device->GetResourceAllocator();
	if (render_setup.IsObject())
	{
		_global_resource_set.AddExternal("back_buffer", _device->GetBackBuffer()); // TODO: What if we have multiple swap-chains?

		// Load global resources
		_global_resource_set.Load(render_setup["global_resources"], resource_allocator);

		// Render views
		if (render_setup["render_views"].IsArray())
		{
			_render_views.Load(render_setup["render_views"], resource_allocator, &_global_resource_set);
		}

		// Load layers
		LoadLayerConfig(render_setup["layers"]);
		LoadShadingEnvironments(render_setup["shading_environments"]);
	}

	_device->FlushAllocator();
}
void Renderer::UnloadRenderSetup()
{
	RenderResourceAllocator* resource_allocator = _device->GetResourceAllocator();

	for (auto& layer : _layers)
	{
		delete layer;
	}
	_layers.clear();

	for (auto& env : _shading_environments)
	{
		delete env.second;
	}
	_shading_environments.clear();

	_render_views.Unload(resource_allocator);

	_global_resource_set.Unload(resource_allocator);
	_global_resource_set.Clear();

	_device->FlushAllocator();
}

//-------------------------------------------------------------------------------

void Renderer::LoadLayerConfig(const ConfigValue& layers)
{
	Assert(layers.IsArray());
	AssertMsg(layers.Size() < 256, "Maximum number of layers");
	for (uint32_t l = 0; l < layers.Size(); ++l)
	{
		const ConfigValue& layer_cfg = layers[l];
		Assert(layer_cfg.IsObject());

		Layer* layer = new Layer();
		Assert(layer);
		layer->sort_key = ((uint64_t)l << render_sorting::LAYER_BIT);

		Assert(layer_cfg["name"].IsString());
		layer->name = layer_cfg["name"].AsString();

		if (layer_cfg["render_targets"].IsString())
		{
			// Split the string as in the case of multiple targets they are separated by spaces
			const char* targets = layer_cfg["render_targets"].AsString();
			uint32_t rt = 0;
			while (*targets != '\0')
			{
				const char* c = targets;
				while (*c != '\0' && *c != ' ') { ++c; }

				layer->render_targets[rt++] = StringId32(targets, uint32_t(c - targets));

				if (*c != '\0') ++c;
				targets = c;
			}
		}

		if (layer_cfg["depth_stencil_target"].IsString())
		{
			layer->depth_stencil_target = layer_cfg["depth_stencil_target"].AsString();
		}

		if (layer_cfg["clear"].IsString())
		{
			// Split the string as in the case of multiple targets they are separated by spaces
			const char* targets = layer_cfg["clear"].AsString();
			while (*targets != '\0')
			{
				const char* c = targets;
				while (*c != '\0' && *c != ' ') { ++c; }

				StringId32 flag = StringId32(targets, uint32_t(c - targets));
				if (*c != '\0') ++c;
				targets = c;

				if (flag == "color")
					layer->clear_flags |= rendering::CLEAR_COLOR;
				else if (flag == "depth")
					layer->clear_flags |= rendering::CLEAR_DEPTH;
				else if (flag == "stencil")
					layer->clear_flags |= rendering::CLEAR_STENCIL;
			}
		}
		if (layer_cfg["depth_sort"].IsString())
		{
			if (strcmp(layer_cfg["depth_sort"].AsString(), "FRONT_BACK"))
			{
				layer->depth_sort = render_sorting::FRONT_TO_BACK;
			}
			else if (strcmp(layer_cfg["depth_sort"].AsString(), "BACK_FRONT"))
			{
				layer->depth_sort = render_sorting::BACK_TO_FRONT;
			}
		}

		// Shader technique
		if (layer_cfg["technique"].IsString())
		{
			layer->technique = layer_cfg["technique"].AsString();
		}
		else
		{
			layer->technique = "default";
		}

		// Generator parameter is optional
		if (layer_cfg["render_view"].IsString())
		{
			layer->render_views.push_back(layer_cfg["render_view"].AsString());
		}
		else if (layer_cfg["render_view"].IsArray())
		{
			for (uint32_t r = 0; r < layer_cfg["render_view"].Size(); ++r)
			{
				Assert(layer_cfg["render_view"][r].IsString());
				layer->render_views.push_back(layer_cfg["render_view"][r].AsString());
			}
		}

		if (layer->render_views.empty())
		{
			logging::Warning("Render layer 0x%x have no specified render views.", layer->name);
		}

		_layers.push_back(layer);
	}
}
void Renderer::LoadShadingEnvironments(const ConfigValue& envs)
{
	if (!envs.IsArray())
		return;

	for (uint32_t i = 0; i < envs.Size(); ++i)
	{
		const ConfigValue& env_cfg = envs[i];
		Assert(env_cfg.IsObject());

		Assert(env_cfg["name"].IsString());
		StringId32 name = env_cfg["name"].AsString();

		ShadingEnvironment* env = new ShadingEnvironment();
		Assert(env);
		env->Load(env_cfg);

		_shading_environments[name] = env;
	}
}

//-------------------------------------------------------------------------------
void Renderer::DrawWorld(World* world, Camera* camera, Viewport* viewport, ShadingEnvironment* shading_env, Camera* /* external_frustum */)
{
	PROFILER_SCOPE("Renderer::DrawWorld");

	RenderWorld* render_world = world->GetRenderWorld();
	RenderContext* render_context = _device->CreateRenderContext();

	camera->Update();
	UpdatePerFrameData(camera, render_context);

	RenderView::Params render_params;
	render_params.device = _device;
	render_params.scheduler = _scheduler;
	render_params.resources = &_global_resource_set;
	render_params.world = render_world;
	render_params.camera = camera;
	render_params.shader_params = &shading_env->GetShaderParameters();
	render_params.viewport = viewport;

	for (auto& layer : _layers)
	{
		render_params.layer = layer;

		uint64_t view_id = 0;
		for (auto& view_name : layer->render_views)
		{
			RenderView* view = _render_views.Get(view_name);
			Assert(view);

			uint64_t sort_key = layer->sort_key | ((view_id++) << render_sorting::VIEW_BIT);
			view->Render(sort_key, render_params, render_context);
		}
	}

	_device->Dispatch(1, &render_context);
	render_context->ClearContext();
		
	_device->ReleaseRenderContext(render_context);

}
void Renderer::FlushGUI(GUICanvas* gui)
{
	RenderContext* render_context = _device->CreateRenderContext();
	RenderResourceAllocator* resource_allocator = _device->GetResourceAllocator();

	RRenderTarget* back_buffer = (RRenderTarget*) _global_resource_set.GetResource("back_buffer");
	Assert(back_buffer);

	render_context->ClearState(0);

	Viewport vp;
	vp.x = vp.y = 0;
	vp.width = (float) back_buffer->GetDesc().width;
	vp.height = (float) back_buffer->GetDesc().height;
	vp.max_depth = 1.0f;
	vp.min_depth = 0.0f;

	RenderContext::Targets targets;
	targets.render_targets[0] = *back_buffer;
	targets.num_viewports = 1;
	targets.viewports = &vp;

	render_context->SetTargets(1, targets);

	gui->Flush(render_context, resource_allocator, 0);

	// Flush the resource allocator
	_device->FlushAllocator();

	_device->Dispatch(1, &render_context);
	render_context->ClearContext();

	_device->ReleaseRenderContext(render_context);
}
void Renderer::UpdatePerFrameData(Camera* camera, RenderContext* render_context)
{
	// Update global constant buffers
	ShaderPerFrameData per_frame_data;
	uint32_t back_buffer_x, back_buffer_y;
	GetBackbufferSize(back_buffer_x, back_buffer_y);

	per_frame_data.back_buffer_size = Vec2f((float)back_buffer_x, (float)back_buffer_y);
	per_frame_data.camera_near_far = Vec2f(camera->GetNearRange(), camera->GetFarRange());
	per_frame_data.camera_world = camera->GetWorldMatrix();
	per_frame_data.camera_view = camera->GetViewMatrix();
	per_frame_data.camera_projection = camera->GetProjMatrix();

	_shader_manager->UpdateConstantBuffers(render_context, per_frame_data);
}
//-------------------------------------------------------------------------------
void Renderer::GetBackbufferSize(uint32_t& width, uint32_t& height) const
{
	RRenderTarget* buf = _device->GetBackBuffer();
	Assert(buf);

	const TextureDesc& desc = buf->GetDesc();
	width = desc.width;
	height = desc.height;
}
//-------------------------------------------------------------------------------
RenderDevice* Renderer::GetDevice()
{
	return _device;
}
ShaderManager* Renderer::GetShaderManager()
{
	return _shader_manager;
}
MaterialManager* Renderer::GetMaterialManager()
{
	return _material_manager;
}
RenderResourceSet* Renderer::GetGlobalResourceSet()
{
	return &_global_resource_set;
}
ResourceManager* Renderer::GetResourceManager()
{
	return _resource_manager;
}

RenderResourceAllocator* Renderer::GetResourceAllocator()
{
	Assert(_device);
	return _device->GetResourceAllocator();
}

const vector<Layer*>& Renderer::GetLayers() const
{
	return _layers;
}
Layer* Renderer::GetLayer(StringId32 name)
{
	for (auto& layer : _layers)
	{
		if (layer->name == name)
			return layer;
	}
	Assert(false);
	return nullptr;
}
ShadingEnvironment* Renderer::GetShadingEnvironment(StringId32 name)
{
	map<StringId32, ShadingEnvironment*>::iterator it = _shading_environments.find(name);
	if (it != _shading_environments.end())
		return it->second;
	return nullptr;
}
//-------------------------------------------------------------------------------

} // namespace sb

