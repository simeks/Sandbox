// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_SHADERMANAGER_H__
#define __RENDERING_SHADERMANAGER_H__

#include "Shader.h"
#include "PerFrameVariableBinder.h"

namespace sb
{

	struct ShaderLibrary;

	class RenderContext;
	class ResourceManager;
	class RenderResourceAllocator;
	class RenderDevice;

	/// @brief Manager class handling shader objects
	class ShaderManager
	{
	public:
		ShaderManager(RenderDevice* device, ResourceManager* resource_manager);
		~ShaderManager();

		Shader* GetShader(StringId32 name);

		void AddLibrary(ShaderLibrary* library);
		void RemoveLibrary(ShaderLibrary* library);

		/// Updates all global constant buffers
		void UpdateConstantBuffers(RenderContext* context, const ShaderPerFrameData& per_frame_data);

		/// Tries to create a global constant buffer from the specified reflection, if a buffer already exists the manager will return that
		///		given that the reflections are matching.
		///	@remark Method will assert if there's two buffers with the same name but different reflections.
		const RConstantBuffer& CreateGlobalConstantBuffer(StringId32 name, const ConstantBufferReflection& reflection);

		/// Releases the specified buffer, destroying it if reference count hits 0
		void ReleaseGlobalConstantBuffer(StringId32 name);

		RenderResourceAllocator* GetResourceAllocator();

	private:
		struct GlobalConstantBuffer
		{
			GlobalConstantBuffer(const ConstantBufferReflection& reflection, const RConstantBuffer& render_resource)
				: render_resource(render_resource), reflection(reflection), refcount(0), data(nullptr), binder(reflection)
			{
			}
			~GlobalConstantBuffer() { }

			RConstantBuffer render_resource;
			ConstantBufferReflection reflection;
			uint32_t refcount;

			void* data;

			PerFrameVariableBinder binder;
		};

		RenderDevice* _device;

		map<StringId32, Shader*> _shaders;
		map<StringId32, GlobalConstantBuffer> _global_constant_buffers;

	};


} // namespace sb


#endif // __RENDERING_SHADERMANAGER_H__

