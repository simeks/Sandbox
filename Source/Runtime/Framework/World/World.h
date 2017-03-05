// Copyright 2008-2014 Simon Ekström

#ifndef __FRAMEWORK_WORLD_H__
#define __FRAMEWORK_WORLD_H__

#include "Transform.h"

namespace sb
{
	class GameObject;
	class RenderWorld;
	class Renderer;
	class World
	{
	public:
		World(Renderer* renderer);
		~World();

		void Update(float dt);

		/// @brief Creates a new empty object.
		/// @sa ReleaseObject
		GameObject* CreateObject();

		/// @brief Releases an object.
		/// @sa CreateObject
		void ReleaseObject(GameObject* object);


		RenderWorld* GetRenderWorld();

	private:
		RenderWorld* _render_world;
		Transform _root_transform;

		vector<GameObject*> _objects;

	};

} // namespace sb



#endif // __FRAMEWORK_WORLD_H__
