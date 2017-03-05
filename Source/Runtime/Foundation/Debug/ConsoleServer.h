// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_CONSOLESERVER_H__
#define __FOUNDATION_CONSOLESERVER_H__

#include "Console.h"

#include <Foundation/Net/Socket.h>

namespace sb
{

	class ConfigValue;

	/// Server listening for and handling connections from the console tool.
	class ConsoleServer
	{
	public:
		/// @param port Specifies which port to listen for connections on
		ConsoleServer(int port);
		~ConsoleServer();

		/// Updates the server, checking for incomining data and connections
		void Update();

		/// Sends a ConfigValue to all clients
		void Send(const ConfigValue& value);

		/// Registers a new command callback for when receiving commands from clients
		/// @param command Command name
		/// @sa RemoveCommand
		void AddCommand(const char* command, console::CommandCallback callback, void* user_data, const char* description);

		/// Unregisters a command registered with AddCommand.
		/// @param command Command name
		/// @sa AddCommand
		void RemoveCommand(const char* command);

	private:
		struct Client
		{
			Socket socket;
		};
		struct Command
		{
			console::CommandCallback callback;
			void* user_data;
			string description;
		};

		/// Sends a welcome message to a client
		void SendHello(Client* client);

		/// Processes an input command
		void ProcessCommand(const ConfigValue& cmd);

		/// Binds the server socket
		void Intialize();

	private:
		Socket _socket;
		int _port;

		vector<Client*> _clients;

		fd_set	_fds;
		int		_fdmax;

		map<string, Command> _commands;

	};

} // namespace sb



#endif // __FOUNDATION_CONSOLESERVER_H__

