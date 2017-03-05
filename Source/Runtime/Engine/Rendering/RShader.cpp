// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "RShader.h"

namespace sb
{

	RShader::RShader(void* platform_data, uint32_t platform_data_size)
		: RenderResource(SHADER),
		_platform_data(platform_data),
		_platform_data_size(platform_data_size)
	{
	}

	RShader::RShader()
		: RenderResource(SHADER),
		_platform_data(nullptr),
		_platform_data_size(0)
	{
	}

	void* RShader::GetPlatformData() const
	{
		return _platform_data;
	}
	uint32_t RShader::GetPlatformDataSize() const
	{
		return _platform_data_size;
	}

} // namespace sb

