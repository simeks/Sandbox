// Copyright 2008-2014 Simon Ekström

#ifndef __FRAMEWORK_RENDERER_H__
#define __FRAMEWORK_RENDERER_H__

#include "Layer.h"
#include "RenderResourceSet.h"
#include "RenderView/RenderViewSet.h"

#include <Engine/Rendering/RenderDevice.h>

#define SANDBOX_RENDER_DEVICE D3D11RenderDevice // NullRenderDevice


namespace sb
{

	class ResourceManager;
	class TaskScheduler;
	class ShaderManager;
	class MaterialManager;
	class ConfigValue;
	class World;
	class GUICanvas;
	class RenderWorld;
	class Camera;
	class ShadingEnvironment;
	class RenderResourceAllocator;

	/// @brief Interface for the render system
	class Renderer
	{
	public:
		Renderer(ResourceManager* resource_manager, TaskScheduler* scheduler);
		~Renderer();

		void Initialize(const RenderDevice::InitParams& params);
		void Shutdown();

		/// Loads a render setup from json, this should be called after all render settings are set up 
		///		and after you have created a swap-chain, as the global resource setup may be dependent
		///		on the back buffer.
		void LoadRenderSetup(const ConfigValue& render_setup);
		void UnloadRenderSetup();

		void DrawWorld(World* world, Camera* camera, Viewport* viewport, ShadingEnvironment* shading_env, Camera* external_frustum = nullptr);
		void FlushGUI(GUICanvas* gui);

		/// Gets the size of the active back buffer
		void GetBackbufferSize(uint32_t& width, uint32_t& height) const;

		RenderDevice* GetDevice();
		ShaderManager* GetShaderManager();
		MaterialManager* GetMaterialManager();
		RenderResourceSet* GetGlobalResourceSet();
		ResourceManager* GetResourceManager();

		RenderResourceAllocator* GetResourceAllocator();

		const vector<Layer*>& GetLayers() const;

		/// @return Layer with the specified name, NULL if none was found.
		Layer* GetLayer(StringId32 name);

		/// @return ShadingEnvironment with the specified name, NULL if none was found.
		ShadingEnvironment* GetShadingEnvironment(StringId32 name);

	private:
		void LoadLayerConfig(const ConfigValue& layers);
		void LoadShadingEnvironments(const ConfigValue& envs);

		void UpdatePerFrameData(Camera* camera, RenderContext* render_context);

		ResourceManager* _resource_manager;
		TaskScheduler* _scheduler;

		RenderDevice* _device;
		ShaderManager* _shader_manager;
		MaterialManager* _material_manager;
		RenderViewSet _render_views;

		vector<Layer*> _layers;
		RenderResourceSet _global_resource_set;

		map<StringId32, ShadingEnvironment*> _shading_environments;

	};

} // namespace sb


#endif // __FRAMEWORK_RENDERER_H__
