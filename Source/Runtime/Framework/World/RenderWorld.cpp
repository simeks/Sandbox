// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "RenderWorld.h"
#include "RenderComponent.h"

#include <Framework/Rendering/Renderer.h>


namespace sb
{

RenderWorld::RenderWorld(Renderer* renderer)
	: _renderer(renderer)
{
}
RenderWorld::~RenderWorld()
{
}

uint32_t RenderWorld::AddObject(RenderComponent* object, const AABB& aabb, uint8_t flags)
{
	Assert(object);

	uint32_t handle;
	if (_free_handles.size())
	{
		handle = _free_handles.back();
		_free_handles.pop_back();

		_bounding_volumes[handle] = aabb;
		_visibility_flags[handle] = flags;
	}
	else
	{
		handle = (uint32_t)_objects.size();

		_objects.push_back(nullptr);
		_bounding_volumes.push_back(aabb);
		_visibility_flags.push_back(flags);

	}

	_objects[handle] = object;

	return handle;
}
void RenderWorld::RemoveObject(uint32_t handle)
{
	Assert(IsValid(handle));
	Assert(size_t(handle) < _objects.size());

	_objects[handle] = nullptr;
	_free_handles.push_back(handle);
}

void RenderWorld::SetAABB(uint32_t object_handle, const AABB& aabb)
{
	Assert(object_handle < _objects.size());
	_bounding_volumes[object_handle] = aabb;
}
void RenderWorld::SetVisibilityFlags(uint32_t object_handle, uint8_t flags)
{
	Assert(object_handle < _objects.size());
	_visibility_flags[object_handle] = flags;
}

const vector<RenderComponent*>& RenderWorld::GetObjects() const
{
	return _objects;
}

} // namespace sb

