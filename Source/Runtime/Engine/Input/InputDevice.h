// Copyright 2008-2014 Simon Ekström

#ifndef __ENGINE_INPUT_DEVICE_H__
#define __ENGINE_INPUT_DEVICE_H__

namespace sb
{

	class InputDevice
	{
	public:
		InputDevice();
		virtual ~InputDevice();

		void Update();
		virtual void Poll() = 0;

		/// Returns true if the specified button is currently down.
		bool IsDown(uint32_t button);

		/// Returns true if the specified button was pressed this frame.
		bool Pressed(uint32_t button);

		/// Returns true if the specified button was released this frame.
		bool Released(uint32_t button);


	protected:
		void SetButtonCount(uint32_t count);
		void ButtonPressed(uint32_t button);
		void ButtonReleased(uint32_t button);

	private:
		vector<bool> _button_state; // Current button state, false = up, true = down
		vector<bool> _pressed_buttons;
		vector<bool> _released_buttons;

	};

} // namespace sb


#endif // __ENGINE_INPUT_DEVICE_H__
