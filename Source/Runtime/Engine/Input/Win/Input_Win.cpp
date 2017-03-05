// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Input_Win.h"
#include "../Input.h"

#include <WindowsX.h>

namespace sb
{

	//-------------------------------------------------------------------------------

	/// @brief Returns the current cursor position 
	Vec2f input::GetMousePosition()
	{
		POINT pt;
		GetCursorPos(&pt);

		return Vec2f((float)pt.x, (float)pt.y);
	}

	/// @brief Changes the cursors position 
	void input::SetMousePosition(const Vec2f& position)
	{
		SetCursorPos((int)position.x, (int)position.y);
	}
	//-------------------------------------------------------------------------------

	void input::RawInput_Initialize(HWND hwnd)
	{
		RAWINPUTDEVICE Rid[2];

		// Mouse
		Rid[0].usUsagePage = 0x01;
		Rid[0].usUsage = 0x02;
		Rid[0].dwFlags = 0;
		Rid[0].hwndTarget = (HWND)hwnd;

		// Keyboard
		Rid[1].usUsagePage = 0x01;
		Rid[1].usUsage = 0x06;
		Rid[1].dwFlags = 0;
		Rid[1].hwndTarget = (HWND)hwnd;

		if (RegisterRawInputDevices(Rid, 2, sizeof(RAWINPUTDEVICE)) == FALSE)
		{
			logging::Error("Failed to register RawInput device");
		}
	}

	//-------------------------------------------------------------------------------

	void InputManager::HandleMessage(HWND, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{
		case WM_CHAR:
		{
			input::KeyEvent evt;
			evt.key = (uint32_t)wparam;

			evt.state = input::KS_CHAR;

			PostKeyEvent(evt);
			break;
		}
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		{
			input::MouseEvent evt;
			evt.state = input::MS_PRESSED;
			evt.position = Vec2f((float)GET_X_LPARAM(lparam), (float)GET_Y_LPARAM(lparam));

			if (msg == WM_LBUTTONDOWN)
			{
				evt.button = input::MB_LEFT;
			}
			else if (msg == WM_RBUTTONDOWN)
			{
				evt.button = input::MB_RIGHT;
			}
			else if (msg == WM_MBUTTONDOWN)
			{
				evt.button = input::MB_MIDDLE;
			}

			PostMouseEvent(evt);
			break;
		}
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
		{
			input::MouseEvent evt;
			evt.state = input::MS_RELEASED;
			evt.position = Vec2f((float)GET_X_LPARAM(lparam), (float)GET_Y_LPARAM(lparam));

			if (msg == WM_LBUTTONUP)
			{
				evt.button = input::MB_LEFT;
			}
			else if (msg == WM_RBUTTONUP)
			{
				evt.button = input::MB_RIGHT;
			}
			else if (msg == WM_MBUTTONUP)
			{
				evt.button = input::MB_MIDDLE;
			}

			PostMouseEvent(evt);
			break;
		}
		case WM_MOUSEMOVE:
		{
			input::MouseEvent evt;
			evt.state = input::MS_MOVEUI;
			evt.position = Vec2f((float)GET_X_LPARAM(lparam), (float)GET_Y_LPARAM(lparam));

			PostMouseEvent(evt);
			break;
		}
		case WM_MOUSEWHEEL:
		{
			input::MouseEvent evt;
			evt.state = input::MS_WHEEL;
			evt.position = Vec2f((float)GET_X_LPARAM(lparam), (float)GET_Y_LPARAM(lparam));
			evt.wheel_delta = (float)GET_WHEEL_DELTA_WPARAM(wparam);

			PostMouseEvent(evt);
			break;
		}
		case WM_INPUT:
		{
			UINT size = 0;

			GetRawInputData((HRAWINPUT)lparam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
			LPBYTE lpb = (LPBYTE)alloca(size);
			
			if (lpb == NULL)
				return;

			if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, lpb, &size, sizeof(RAWINPUTHEADER)) != size)
				logging::Warning("GetRawInputData does not return the correct size!");

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				input::KeyEvent evt;
				evt.key = raw->data.keyboard.MakeCode;
				if (raw->data.keyboard.Message == WM_KEYDOWN)
				{
					evt.state = input::KS_PRESSED;
				}
				else if (raw->data.keyboard.Message == WM_KEYUP)
				{
					evt.state = input::KS_RELEASED;
				}
				PostKeyEvent(evt);
			}
			else if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				if (raw->data.mouse.lLastX || raw->data.mouse.lLastY)
				{
					input::MouseEvent evt;
					evt.state = input::MS_MOVE;
					evt.delta = Vec2f((float)raw->data.mouse.lLastX, (float)raw->data.mouse.lLastY);
					evt.position = input::GetMousePosition();

					PostMouseEvent(evt);
				}

				USHORT button_flags = raw->data.mouse.usButtonFlags;
				if (button_flags & RI_MOUSE_BUTTON_4_DOWN)
				{
					input::MouseEvent evt;
					evt.state = input::MS_PRESSED;
					evt.button = input::MB_BUTTON4;
					evt.position = input::GetMousePosition();

					PostMouseEvent(evt);
				}
				if (button_flags & RI_MOUSE_BUTTON_4_UP)
				{
					input::MouseEvent evt;
					evt.state = input::MS_RELEASED;
					evt.button = input::MB_BUTTON4;
					evt.position = input::GetMousePosition();

					PostMouseEvent(evt);
				}
				if (button_flags & RI_MOUSE_BUTTON_5_DOWN)
				{
					input::MouseEvent evt;
					evt.state = input::MS_PRESSED;
					evt.button = input::MB_BUTTON5;
					evt.position = input::GetMousePosition();

					PostMouseEvent(evt);
				}
				if (button_flags & RI_MOUSE_BUTTON_5_UP)
				{
					input::MouseEvent evt;
					evt.state = input::MS_RELEASED;
					evt.button = input::MB_BUTTON5;
					evt.position = input::GetMousePosition();

					PostMouseEvent(evt);
				}
			}

			break;
		}
		};

	}
	//-------------------------------------------------------------------------------
	Win32Keyboard::Win32Keyboard()
	{
		SetButtonCount(256);
	}
	Win32Keyboard::~Win32Keyboard()
	{
	}
	//-------------------------------------------------------------------------------
	void Win32Keyboard::Poll()
	{
		for (auto& evt : _events)
		{
			if (evt.state & input::KS_PRESSED)
				ButtonPressed(evt.key);
			else if (evt.state & input::KS_RELEASED)
				ButtonReleased(evt.key);

		}
		_events.clear();
	}
	//-------------------------------------------------------------------------------
	void Win32Keyboard::OnKeyEvent(const input::KeyEvent& evt)
	{
		_events.push_back(evt);
	}


	//-------------------------------------------------------------------------------
	Win32Mouse::Win32Mouse(HWND hwnd) : _hwnd(hwnd)
	{
		SetButtonCount(5);
	}
	Win32Mouse::~Win32Mouse()
	{
	}
	//-------------------------------------------------------------------------------
	void Win32Mouse::Poll()
	{
		for (auto& evt : _button_events)
		{
			if (evt.state == input::MS_PRESSED)
				ButtonPressed(evt.button);
			else if (evt.state == input::MS_RELEASED)
				ButtonReleased(evt.button);
		}

		SetAxisValue(_axis);

		_button_events.clear();
		_axis = Vec3f(0, 0, 0);

		Vec2f pos = input::GetMousePosition();
		POINT pt;
		pt.x = (LONG)pos.x;
		pt.y = (LONG)pos.y;
		ScreenToClient(_hwnd, &pt);

		_position = Vec2f((float) pt.x, (float) pt.y);
	}
	//-------------------------------------------------------------------------------
	void Win32Mouse::OnMouseEvent(const input::MouseEvent& evt)
	{
		if ((evt.state == input::MS_PRESSED) || (evt.state == input::MS_RELEASED))
		{
			_button_events.push_back(evt);
		}
		else if (evt.state == input::MS_MOVE)
		{
			_axis.x += evt.delta.x;
			_axis.y += evt.delta.y;
		}
		else if (evt.state == input::MS_WHEEL)
		{
			_axis.z += evt.wheel_delta;
		}
	}
	//-------------------------------------------------------------------------------

} // namespace sb

