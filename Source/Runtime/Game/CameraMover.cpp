// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "CameraMover.h"

#include <Engine/Input/input.h>
#include <Framework/World/GameObject.h>
#include <Framework/World/Camera.h>
#include <Foundation/Math/Math.h>



namespace sb
{

	CameraMover::CameraMover(Keyboard* keyboard, Mouse* mouse, GameObject* _camera_entity)
		: _keyboard(keyboard),
		_mouse(mouse),
		_camera_entity(_camera_entity)
	{
		_angle = Vec2f(0, 0);
	}
	CameraMover::~CameraMover()
	{
	}
	void CameraMover::Update(float dtime)
	{
		Vec3f vel(0, 0, 0);
		Vec2f rot_vel(0, 0);

		float speed = 5.0f;
		if (_keyboard->IsDown(input::KC_LSHIFT))
			speed = 10.0f;

		if (_keyboard->IsDown(input::KC_W))
		{
			vel.y += speed; // 100 units / second
		}
		if (_keyboard->IsDown(input::KC_S))
		{
			vel.y -= speed; // 100 units / second
		}
		if (_keyboard->IsDown(input::KC_A))
		{
			vel.x += speed; // 100 units / second
		}
		if (_keyboard->IsDown(input::KC_D))
		{
			vel.x -= speed; // 100 units / second
		}

		if (_mouse->Pressed(input::MB_LEFT))
			_cursor_pos = input::GetMousePosition();

		if (_mouse->IsDown(input::MB_LEFT))
		{
			rot_vel = Vec2f(_mouse->Axis().x, _mouse->Axis().y) * 0.01f;
			_angle += rot_vel;
			_angle.y = Max<float>((float)-MATH_PI / 2.0f, _angle.y);
			_angle.y = Min<float>((float)MATH_PI / 2.0f, _angle.y);

			input::SetMousePosition(_cursor_pos);
		}

		Mat3x3f rot_x = Mat3x3f::CreateRotationX(_angle.y);
		Mat3x3f rot_z = Mat3x3f::CreateRotationZ(_angle.x);

		Mat3x3f rot = rot_x * rot_z;

		Vec3f up(0, 0, 1);
		Vec3f forward(0, 1, 0);
		up = up * rot;
		forward = forward * rot;
		up.Normalize();
		forward.Normalize();

		Vec3f right = up.Cross(forward);
		right.Normalize();

		Vec3f pos = _camera_entity->GetTransform().GetLocalPosition();
		pos += forward * (vel.y * dtime);
		pos += right * (vel.x * dtime);

		_camera_entity->GetTransform().SetLocalPosition(pos);
		_camera_entity->GetTransform().SetLocalRotation(Mat3x3f::CreateOrientation(forward, up));
	}

} // namespace sb

