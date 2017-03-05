// Copyright 2008-2014 Simon Ekström

#ifndef __ENGINE_INPUT_H__
#define __ENGINE_INPUT_H__

#include <Foundation/Math/Vec2.h>

#include "Win/Input_Win.h"

namespace sb
{

	class Keyboard;
	class Mouse;

	namespace input
	{
		//-------------------------------------------------------------------------------
#ifdef WIN32
		/// @brief Initializes RawInput for mouse and keyboard
		void RawInput_Initialize(HWND hwnd);
#endif

		/// @brief Returns the current cursor position 
		Vec2f GetMousePosition();

		/// @brief Changes the cursors position 
		void SetMousePosition(const Vec2f& position);

		//-------------------------------------------------------------------------------


	}; // namespace input


	/// @brief Used to handle input messages
	class InputManager
	{
	public:
		InputManager(HWND hwnd);
		~InputManager();

		void Update();

#ifdef WIN32
		/// @brief Handling incoming window messages
		///
		///	Should be called when receiving window messages so that the input system
		///	can handle rawinput messages.
		void HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
#endif

		Keyboard* GetKeyboard() const;
		Mouse* GetMouse() const;

	private:
		Win32Keyboard* _keyboard;
		Win32Mouse* _mouse;

		void PostKeyEvent(const input::KeyEvent& evt);
		void PostMouseEvent(const input::MouseEvent& evt);

	};

} // namespace sb


#endif // __ENGINE_INPUT_H__
