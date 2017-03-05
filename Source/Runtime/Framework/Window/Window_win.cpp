// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Window.h"

#include <Windowsx.h>



namespace sb
{

	LRESULT CALLBACK InternalWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

#define WINDOW_CLASSNAME TEXT("SandboxWnd")

	namespace
	{
		bool g_window_class_registered = false;

		void RegisterWndClass()
		{
			if (g_window_class_registered) // Already registered
				return;

			WNDCLASSEX wc;
			ZeroMemory(&wc, sizeof(WNDCLASSEX));

			wc.cbSize = sizeof(WNDCLASSEX);
			wc.lpfnWndProc = InternalWndProc;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 2 * sizeof(LONG_PTR);
			wc.hInstance = GetModuleHandle(0);
			wc.hIcon = LoadIcon(0, IDI_APPLICATION);
			wc.hCursor = LoadCursor(0, IDC_ARROW);
			wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
			wc.lpszMenuName = NULL;
			wc.lpszClassName = WINDOW_CLASSNAME;
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.hIconSm = LoadIcon(0, IDI_APPLICATION);


			if (!::RegisterClassEx(&wc))
			{
				int err = GetLastError();
				logging::Warning("Failed to create window class: %d", err);
			}

			g_window_class_registered = true;
		}
		void UnregisterWndClass()
		{
			if (!g_window_class_registered) // No class registered
				return;

			::UnregisterClass(WINDOW_CLASSNAME, GetModuleHandle(0));

			g_window_class_registered = false;
		}

	};



	//-------------------------------------------------------------------------------
	Window::Window()
	{
		_width = _height = 0;
		_left = _top = 0;
		_hwnd = 0;

	}
	Window::~Window()
	{
		Destroy();
	}
	//-------------------------------------------------------------------------------
	void Window::SetMessageCallback(MessageCallback callback, void* data)
	{
		SetWindowLongPtr(_hwnd, 0, (LONG_PTR)callback);
		SetWindowLongPtr(_hwnd, sizeof(LONG_PTR), (LONG_PTR)data);
	}
	//-------------------------------------------------------------------------------
	void Window::Create(const char* caption, int width, int height, bool fullscreen)
	{
		if (g_window_class_registered == false)
		{
			RegisterWndClass();
		}
		if (_hwnd)
		{
			Destroy();
		}

		int	win_width, win_height;

		_width = width;
		_height = height;


		DWORD style = (WS_CLIPCHILDREN | WS_VISIBLE);
		DWORD style_ex = 0;

		win_width = width;
		win_height = height;

		// Center window
		_left = (GetSystemMetrics(SM_CXSCREEN) - win_width) / 2;
		_top = (GetSystemMetrics(SM_CYSCREEN) - win_height) / 2;

		if (fullscreen)
		{
			style_ex |= WS_EX_TOPMOST;
			style |= WS_POPUP;
			_top = _left = 0;
		}
		else
		{
			// TODO: Add more window options 
			style |= WS_OVERLAPPEDWINDOW;
		}

		RECT rc;
		SetRect(&rc, 0, 0, width, height);

		AdjustWindowRect(&rc, style, FALSE);

		win_width = rc.right - rc.left;
		win_height = rc.bottom - rc.top;

		SetRect(&rc, 0, 0, _width, _height);
		AdjustWindowRect(&rc, style, false);
		win_width = rc.right - rc.left;
		win_height = rc.bottom - rc.top;


		_hwnd = CreateWindowEx(
			style_ex,
			WINDOW_CLASSNAME,
			caption,
			style,
			_left,
			_top,
			win_width,
			win_height,
			NULL,
			NULL,
			NULL,
			this);


		if (_hwnd)
		{
			ShowWindow(_hwnd, SW_SHOWNORMAL);
			UpdateWindow(_hwnd);
		}
		else
		{
			int err = GetLastError();
			logging::Warning("Failed to create window: %d", err);
		}

	}
	void Window::Destroy()
	{
		if (_hwnd)
		{
			DestroyWindow(_hwnd);
			_hwnd = 0;
		}
	}
	void Window::SetCaption(const char* caption)
	{
		SetWindowText(_hwnd, caption);
	}
	//-------------------------------------------------------------------------------
	LRESULT CALLBACK InternalWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		Window::MessageCallback callback = (Window::MessageCallback)GetWindowLongPtr(hwnd, 0);
		void* data = (void*)GetWindowLongPtr(hwnd, sizeof(LONG_PTR));

		// Use custom message handler if defined
		if (callback)
		{
			return callback(data, hwnd, msg, wparam, lparam);
		}
		else
		{
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
	}
	//-------------------------------------------------------------------------------


} // namespace sb

