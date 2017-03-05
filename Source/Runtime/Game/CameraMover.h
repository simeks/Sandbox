// Copyright 2008-2014 Simon Ekström

#ifndef __GAME_CAMERAMOVER_H__
#define __GAME_CAMERAMOVER_H__

#include <Foundation/Math/Vec3.h>
#include <Foundation/Math/Vec2.h>


namespace sb
{

	class GameObject;
	class Keyboard;
	class Mouse;

	class CameraMover
	{
	public:
		CameraMover(Keyboard* keyboard, Mouse* mouse, GameObject* camera_entity);
		~CameraMover();

		void Update(float dtime);

	private:
		Keyboard* _keyboard;
		Mouse* _mouse;
		GameObject* _camera_entity;

		Vec2f _angle;
		Vec2f _cursor_pos;
	};

} // namespace sb


#endif // __GAME_CAMERAMOVER_H__
