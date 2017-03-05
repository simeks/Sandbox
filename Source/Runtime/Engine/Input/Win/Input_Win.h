// Copyright 2008-2014 Simon Ekström

#ifndef __ENGINE_INPUT_WIN_H__
#define __ENGINE_INPUT_WIN_H__

#include <engine/input/InputKeyboard.h>
#include <engine/input/InputMouse.h>

namespace sb
{

	class Win32Keyboard : public Keyboard
	{
	public:
		Win32Keyboard();
		~Win32Keyboard();

		void Poll();

		void OnKeyEvent(const input::KeyEvent& evt);

	private:
		vector<input::KeyEvent> _events;


	};

	class Win32Mouse : public Mouse
	{
	public:
		Win32Mouse(HWND hwnd);
		~Win32Mouse();

		void Poll();

		void OnMouseEvent(const input::MouseEvent& evt);

	private:
		HWND _hwnd;
		vector<input::MouseEvent> _button_events;
		Vec3f _axis;

	};

} // namespace sb


#endif // __ENGINE_INPUT_WIN_H__
