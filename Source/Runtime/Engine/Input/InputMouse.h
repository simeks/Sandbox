// Copyright 2008-2014 Simon Ekström

#ifndef __ENGINE_INPUT_MOUSE_H__
#define __ENGINE_INPUT_MOUSE_H__

#include <Foundation/Math/Vec2.h>
#include <Foundation/Math/Vec3.h>

#include "InputDevice.h"

namespace sb
{

	namespace input
	{
		enum MouseState
		{
			MS_UNKNOWN = 0,
			MS_PRESSED = 1,	///< Mouse button pressed, see MouseButton
			MS_RELEASED = 2,	///< Mouse button released, see MouseButton
			MS_MOVE = 3,	///< Mouse moved
			MS_MOVEUI = 4,	///< Mouse moved, this input may have mouse acceleration and should only be used for UI.
			MS_WHEEL = 5,	///< Mouse wheel moved	
		};

		enum MouseButton
		{
			MB_UNKNOWN = 0,
			MB_LEFT = 1,
			MB_RIGHT = 2,
			MB_MIDDLE = 3,
			MB_BUTTON4 = 4,
			MB_BUTTON5 = 5
		};

		struct MouseEvent
		{
			MouseState	state;
			MouseButton button;

			Vec2f		position;
			Vec2f		delta;

			float		wheel_delta;

			MouseEvent()
			{
				state = MS_UNKNOWN;
				button = MB_UNKNOWN;

				position = Vec2f(0.0f, 0.0f);
				delta = Vec2f(0.0f, 0.0f);
				wheel_delta = 0.0f;
			}
		};

	}; // namespace input

	class Mouse : public InputDevice
	{
	public:
		Mouse();
		~Mouse();

		/// Returns the delta values for the mouse for this frame.
		/// X and Y is the delta values for the mouse movement, without acceleration.
		/// Z is the scroll delta-value.
		const Vec3f& Axis() const;

		/// Return mouse position relative to the primary window.
		const Vec2f& Position() const;

	protected:
		void SetAxisValue(const Vec3f& axis);

		Vec3f _axis;
		Vec2f _position;

	};

} // namespace sb



#endif // __ENGINE_INPUT_MOUSE_H__
