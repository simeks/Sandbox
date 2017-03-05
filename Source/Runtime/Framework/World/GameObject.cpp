// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "GameObject.h"
#include "Component.h"
#include "World.h"


namespace sb
{

	//-------------------------------------------------------------------------------

	GameObject::GameObject(World* world)
		: _world(world)
	{
	}
	GameObject::~GameObject()
	{
		for (auto& component : _components)
		{
			component.second->UnregisterComponent();
			delete component.second;
		}
		_components.clear();
	}

	const Transform& GameObject::GetTransform() const
	{
		return _root_transform;
	}
	Transform& GameObject::GetTransform()
	{
		return _root_transform;
	}
	World* GameObject::GetWorld()
	{
		return _world;
	}

	void GameObject::AddComponent(StringId32 name, Component* component)
	{
		map<StringId32, Component*>::iterator it = _components.find(name);
		Assert(it == _components.end());

		_components[name] = component;

		component->SetOwner(this);
		component->RegisterComponent();
	}


	//-------------------------------------------------------------------------------

} // namespace sb

