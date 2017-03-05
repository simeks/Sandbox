// Copyright 2008-2014 Simon Ekström

#ifndef __FRAMEWORK_RENDERVIEW_H__
#define __FRAMEWORK_RENDERVIEW_H__


namespace sb
{

	class RenderDevice;
	class TaskScheduler;
	class RenderResourceSet;
	class Camera;
	class ShaderParameters;
	class RenderWorld;
	class RenderContext;
	class RenderResourceAllocator;
	class ConfigValue;

	struct Layer;
	struct Viewport;

	class RenderView
	{
	public:
		struct Params
		{
			RenderDevice* device;
			TaskScheduler* scheduler;
			RenderResourceSet* resources;

			Camera* camera;
			Viewport* viewport;
			Layer* layer;
			ShaderParameters* shader_params;

			RenderWorld* world;

		};
		virtual ~RenderView() {}

		virtual void Load(const ConfigValue& ,
						  RenderResourceAllocator* ,
						  RenderResourceSet* ) {}

		virtual void Unload(RenderResourceAllocator*) {}

		virtual void Render(uint64_t sort_key, const Params& params, RenderContext* render_context) = 0;

	};

} // namespace sb


#endif // __FRAMEWORK_RENDERVIEW_H__
