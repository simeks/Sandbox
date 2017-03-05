// Copyright 2008-2014 Simon Ekström

#ifndef __FRAMEWORK_RENDERWORLD_H__
#define __FRAMEWORK_RENDERWORLD_H__

#include <Engine/Rendering/culling/Culling.h>
#include <Foundation/Math/AABB.h>

namespace sb
{
	class Renderer;
	class RenderComponent;

	/// @brief A render representation of a world, manages all renderables in a world
	class RenderWorld
	{
	public:
		RenderWorld(Renderer* renderer);
		~RenderWorld();

		uint32_t AddObject(RenderComponent* object, const AABB& aabb, uint8_t flags);
		void RemoveObject(uint32_t handle);

		void SetAABB(uint32_t object_handle, const AABB& aabb);
		void SetVisibilityFlags(uint32_t object_handle, uint8_t flags);

		const vector<RenderComponent*>& GetObjects() const;

	private:
		Renderer* _renderer;

		vector<RenderComponent*> _objects;
		vector<AABB> _bounding_volumes;
		vector<uint8_t> _visibility_flags;

		vector<uint32_t> _free_handles;
	};

} // namespace sb



#endif // __FRAMEWORK_RENDERWORLD_H__
