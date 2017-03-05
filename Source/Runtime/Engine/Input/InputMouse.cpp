// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "InputMouse.h"

namespace sb
{

	//-------------------------------------------------------------------------------
	Mouse::Mouse()
	{
	}
	Mouse::~Mouse()
	{
	}
	//-------------------------------------------------------------------------------
	const Vec3f& Mouse::Axis() const
	{
		return _axis;
	}
	const Vec2f& Mouse::Position() const
	{
		return _position;
	}

	//-------------------------------------------------------------------------------
	void Mouse::SetAxisValue(const Vec3f& axis)
	{
		_axis = axis;
	}
	//-------------------------------------------------------------------------------

} // namespace sb
