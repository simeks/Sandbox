// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "Console.h"
#include "ConsoleServer.h"

namespace sb
{

	namespace
	{
		ConsoleServer* g_server = nullptr;

	};

	void console::Initialize(int port)
	{
		if (g_server != nullptr)
			return;

		g_server = new ConsoleServer(port);
	}

	void console::Shutdown()
	{
		if (g_server == nullptr)
			return;

		delete g_server;
		g_server = nullptr;
	}
	bool console::Initialized()
	{
		return (g_server != nullptr);
	}

	void console::AddCommand(const char* command, CommandCallback callback, void* user_data, const char* description)
	{
		if (g_server)
		{
			g_server->AddCommand(command, callback, user_data, description);
		}
	}
	void console::RemoveCommand(const char* command)
	{
		if (g_server)
		{
			g_server->RemoveCommand(command);
		}
	}

	ConsoleServer* console::Server()
	{
		return g_server;
	}

} // namespace sb
