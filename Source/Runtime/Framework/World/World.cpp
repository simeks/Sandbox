// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "World.h"
#include "RenderWorld.h"
#include "GameObject.h"


namespace sb
{

	World::World(Renderer* renderer)
		: _render_world(nullptr)
	{
		_render_world = new RenderWorld(renderer);
	}

	World::~World()
	{
		// Destroy remaining entities

		for (auto& object : _objects)
		{
			delete object;
		}
		_objects.clear();

		delete _render_world;
		_render_world = nullptr;
	}

	void World::Update(float)
	{
		_root_transform.Update();
	}

	GameObject* World::CreateObject()
	{
		GameObject* object = new GameObject(this);
		_objects.push_back(object);

		_root_transform.AttachChild(&object->GetTransform());

		return object;
	}

	void World::ReleaseObject(GameObject* object)
	{
		vector<GameObject*>::iterator it = std::find(_objects.begin(), _objects.end(), object);
		if (it != _objects.end())
		{
			object->GetTransform().Detach();
			delete object;
			_objects.erase(it);
		}
	}

	RenderWorld* World::GetRenderWorld()
	{
		return _render_world;
	}

} // namespace sb



