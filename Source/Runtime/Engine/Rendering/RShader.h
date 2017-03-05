// Copyright 2008-2014 Simon Ekström

#ifndef __RENDERING_RSHADER_H__
#define __RENDERING_RSHADER_H__

#include "RenderResource.h"

namespace sb
{

	class RConstantBuffer;

	class RShader : public RenderResource
	{
	public:
		RShader();
		RShader(void* platform_data, uint32_t platform_data_size);

		void* GetPlatformData() const;
		uint32_t GetPlatformDataSize() const;

	private:
		void* _platform_data;
		uint32_t _platform_data_size;

	};

	struct ShaderResources
	{
		RenderResource* resources;
		uint32_t num_resources;

		RConstantBuffer* constant_buffers;
		uint32_t num_constant_buffers;

		uint32_t instance_data_size;

		void* constant_buffer_data; // Constant buffer data, but only contains data for non-global buffers.
		void* instance_data;

		ShaderResources() 
			: resources(nullptr), 
			  num_resources(0), 
			  constant_buffers(nullptr), 
			  num_constant_buffers(0),
			  instance_data_size(0),
			  constant_buffer_data(nullptr),
			  instance_data(nullptr)
		{
		}
	};

	struct ShaderContext
	{
		RShader* shader;
		ShaderResources* resources;

		ShaderContext() : shader(nullptr), resources(nullptr) {}
	};


} // namespace sb



#endif // __RENDERING_RSHADER_H__
