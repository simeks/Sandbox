// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "InputDevice.h"

namespace sb
{

	//-------------------------------------------------------------------------------
	InputDevice::InputDevice()
	{
	}
	InputDevice::~InputDevice()
	{

	}
	//-------------------------------------------------------------------------------
	void InputDevice::Update()
	{
		std::fill(_pressed_buttons.begin(), _pressed_buttons.end(), false);
		std::fill(_released_buttons.begin(), _released_buttons.end(), false);

		Poll();
	}

	//-------------------------------------------------------------------------------
	bool InputDevice::IsDown(uint32_t button)
	{
		return _button_state[button];
	}
	bool InputDevice::Pressed(uint32_t button)
	{
		return _pressed_buttons[button];
	}
	bool InputDevice::Released(uint32_t button)
	{
		return _released_buttons[button];
	}

	//-------------------------------------------------------------------------------
	void InputDevice::SetButtonCount(uint32_t count)
	{
		_button_state.resize(count);
		_pressed_buttons.resize(count);
		_released_buttons.resize(count);

		std::fill(_button_state.begin(), _button_state.end(), false);
		std::fill(_pressed_buttons.begin(), _pressed_buttons.end(), false);
		std::fill(_released_buttons.begin(), _released_buttons.end(), false);
	}
	void InputDevice::ButtonPressed(uint32_t button)
	{
		_pressed_buttons[button] = true;
		_button_state[button] = true;
	}
	void InputDevice::ButtonReleased(uint32_t button)
	{
		_released_buttons[button] = true;
		_button_state[button] = false;
	}
	//-------------------------------------------------------------------------------

} // namespace sb

