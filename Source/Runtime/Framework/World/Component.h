// Copyright 2008-2014 Simon Ekström

#ifndef __FRAMEWORK_COMPONENT_H__
#define __FRAMEWORK_COMPONENT_H__


namespace sb
{

	class GameObject;
	class Component
	{
	public:
		Component();
		virtual ~Component();

		virtual void RegisterComponent() {}
		virtual void UnregisterComponent() {}

		/// @brief Sets the owner game object
		void SetOwner(GameObject* object);

	protected:
		GameObject* _game_object;

	};

} // namespace sb



#endif // __FRAMEWORK_COMPONENT_H__
