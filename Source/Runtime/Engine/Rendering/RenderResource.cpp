// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "RenderResource.h"

namespace sb
{

	//-------------------------------------------------------------------------------
	RenderResource::RenderResource() : _type(UNKNOWN), _handle(Invalid<uint32_t>())
	{
	}
	RenderResource::RenderResource(Type type) : _type(type), _handle(Invalid<uint32_t>())
	{
	}
	RenderResource::RenderResource(Type type, uint32_t handle) : _type(type), _handle(handle)
	{
	}
	uint32_t RenderResource::GetHandle() const
	{
		return _handle;
	}
	void RenderResource::SetHandle(uint32_t handle)
	{
		_handle = handle;
	}
	RenderResource::Type RenderResource::GetType() const
	{
		return _type;
	}
	bool RenderResource::IsValid() const
	{
		return sb::IsValid<uint32_t>(_handle);
	}

	//-------------------------------------------------------------------------------

} // namespace sb

