// Copyright 2008-2014 Simon Ekström

#ifndef __FRAMEWORK_WINDOW_H__
#define __FRAMEWORK_WINDOW_H__



namespace sb
{

	class Window
	{
	public:
#ifdef SANDBOX_PLATFORM_WIN
		typedef LRESULT(*MessageCallback)(void*, HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
		HWND _hwnd;
#endif

		int	_width;
		int	_height;
		int	_left;
		int	_top;


		Window();
		~Window();

		void	Create(const char* caption, int width, int height, bool fullscreen = false);
		void	Destroy();

		void	SetCaption(const char* caption);

		void	SetMessageCallback(MessageCallback callback, void* data = 0);


	};

} // namespace sb


#endif // __FRAMEWORK_WINDOW_H__
