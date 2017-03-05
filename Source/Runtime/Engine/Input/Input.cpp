// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Input.h"

namespace sb
{

	//-------------------------------------------------------------------------------
	InputManager::InputManager(HWND hwnd)
	{
		// Initialize input 
		input::RawInput_Initialize(hwnd);

		_keyboard = new Win32Keyboard();
		_mouse = new Win32Mouse(hwnd);
	}
	InputManager::~InputManager()
	{
		delete _mouse;
		delete _keyboard;
	}
	//-------------------------------------------------------------------------------
	void InputManager::Update()
	{
		_keyboard->Update();
		_mouse->Update();
	}
	//-------------------------------------------------------------------------------
	void InputManager::PostKeyEvent(const input::KeyEvent& evt)
	{
		_keyboard->OnKeyEvent(evt);
	}
	void InputManager::PostMouseEvent(const input::MouseEvent& evt)
	{
		_mouse->OnMouseEvent(evt);
	}

	//-------------------------------------------------------------------------------

	Keyboard* InputManager::GetKeyboard() const
	{
		return _keyboard;
	}
	Mouse* InputManager::GetMouse() const
	{
		return _mouse;
	}

	//-------------------------------------------------------------------------------


} // namespace sb


