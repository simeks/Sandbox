// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Component.h"


namespace sb
{

	Component::Component()
		: _game_object(nullptr)
	{
	}
	Component::~Component()
	{
	}
	void Component::SetOwner(GameObject* object)
	{
		_game_object = object;
	}

} // namespace sb


