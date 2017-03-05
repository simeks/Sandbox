// Copyright 2008-2014 Simon Ekström

#ifndef __FRAMEWORK_GAMEOBJECT_H__
#define __FRAMEWORK_GAMEOBJECT_H__

#include "Transform.h"


namespace sb
{

	class World;
	class Component;
	class Transform;
	class GameObject
	{
	public:
		GameObject(World* world);
		~GameObject();

		template<typename T>
		T* CreateComponent(StringId32 name);

		const Transform& GetTransform() const;
		Transform& GetTransform();

		World* GetWorld();

	private:
		void AddComponent(StringId32 name, Component* component);

		World* _world;

		Transform _root_transform;

		map<StringId32, Component*> _components;

	};


	template<typename T>
	T* GameObject::CreateComponent(StringId32 name)
	{
		T* component = new T();
		AddComponent(name, component);
		return component;
	}

} // namespace sb



#endif // __FRAMEWORK_GAMEOBJECT_H__
