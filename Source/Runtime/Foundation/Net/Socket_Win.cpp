// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include <WinSock2.h>
#include "Socket.h"


namespace sb
{

	namespace
	{
		WSADATA g_wsa_data;
		uint32_t g_count;
	};


	void Socket::InitalizeSockets()
	{
		if (++g_count == 1)
		{
			// First intialization
			int res = WSAStartup(0x0202, &g_wsa_data);
			if (res != NO_ERROR)
			{
				logging::Error("Failed to initialize WinSock (Error code: %d)", WSAGetLastError());
				return;
			}
		}
	}
	void Socket::ShutdownSockets()
	{
		if (--g_count == 0)
		{
			// Last initialization call
			WSACleanup();
		}
	}
	int Socket::GetLastError()
	{
		return WSAGetLastError();
	}

} // namespace sb



