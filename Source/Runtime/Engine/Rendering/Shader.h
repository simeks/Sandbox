// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_SHADER_H__
#define __RENDERING_SHADER_H__

#include <Engine/Rendering/RConstantBuffer.h>

#include "ShaderVariable.h"
#include "ShaderResourceBinder.h"

namespace sb
{

	struct ShaderResources;
	struct ShaderContext;

	class RShader;
	class Stream;

	//-------------------------------------------------------------------------------

	struct ShaderResourceReflection
	{
		struct Resource
		{
			StringId32 name;
			uint32_t index;
		};

		ShaderResourceReflection() {}
		~ShaderResourceReflection() {}

		vector<Resource> resources;
	};

	//-------------------------------------------------------------------------------

	struct ConstantBufferReflection
	{
		ConstantBufferReflection();
		~ConstantBufferReflection();

		vector<ShaderVariable> variables;
		uint32_t size;
		RConstantBuffer::Type type;

		StringId32 name;

		void Serialize(Stream& stream);
		void Deserialize(Stream& stream);
	};

	struct ConstantBufferTemplate
	{
		RConstantBuffer render_resource; // Allocated resource
		void* data; // Template data
	};

	struct InstanceDataReflection
	{
		InstanceDataReflection();
		~InstanceDataReflection();

		vector<ShaderVariable> variables;
		uint32_t size;

		void Serialize(Stream& stream);
		void Deserialize(Stream& stream);
	};

	struct ShaderData
	{
		struct Pass
		{
			StringId32 technique;
			bool instanced;
		};

		StringId32 name;
		ShaderResourceReflection resource_reflection;

		vector<ConstantBufferReflection> constant_buffer_reflections;
		vector<ConstantBufferTemplate> constant_buffer_templates;

		InstanceDataReflection instance_data_reflection;

		vector<Pass> passes;

		vector<uint8_t> platform_data; // Platform specific shader data

		ShaderData();
		~ShaderData();

	};

	//-------------------------------------------------------------------------------

	class Shader
	{
	public:
		Shader(ShaderData* data, RShader* render_resource);
		~Shader();

		ShaderContext* CreateContext();
		void ReleaseContext(ShaderContext* context);

		const ShaderResourceBinder& GetShaderResourceBinder() const;
		ShaderResourceBinder& GetShaderResourceBinder();

		const ShaderData* GetData() const;

	private:
		ShaderResources* AllocateResourceBlock();
		void ReleaseResourceBlock(ShaderResources* resources);


		ShaderData* _data;
		RShader* _render_resource;

		vector<ShaderResources*> _resource_blocks;

		ShaderResourceBinder _resource_binder;

	};


} // namespace sb


#endif // __RENDERING_SHADER_H__

