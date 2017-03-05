// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#include "ConsoleServer.h"
#include "Json/Json.h"
#include "Filesystem/FilePath.h"
#include "Container/ConfigValue.h"

namespace sb
{

	//-------------------------------------------------------------------------------
	ConsoleServer::ConsoleServer(int port)
		: _port(port)
	{
		Intialize();
	}

	ConsoleServer::~ConsoleServer()
	{
		// Disconnect all connected clients

		for (auto& client : _clients)
		{
			if (client == NULL)
				continue;

			client->socket.Close();
			delete client;
			client = NULL;
		}
		_clients.clear();
		_socket.Close();
	}
	//-------------------------------------------------------------------------------
	void ConsoleServer::Intialize()
	{
		_socket.Create(Socket::TCP);
		_socket.SetBlocking(false);
		if (!_socket.Bind(SocketAddress(IPAddress("127.0.0.1"), _port)))
		{
			logging::Warning("ConsoleServer: Failed to bind to port %d (Error: %d)", _port, Socket::GetLastError());
			return;
		}
		if (!_socket.Listen())
		{
			logging::Warning("ConsoleServer: Failed to listen to port %d (Error: %d)", _port, Socket::GetLastError());
			return;
		}

		FD_ZERO(&_fds);
		FD_SET(_socket.GetHandle(), &_fds);
		_fdmax = (int)_socket.GetHandle();
	}
	//-------------------------------------------------------------------------------
	void ConsoleServer::Update()
	{
		string str_addr;
		fd_set rfds;

		rfds = _fds;
		if (Socket::Select(_fdmax, &rfds, 0, 0) == SOCKET_ERROR)
		{
			logging::Warning("ConsoleServer: Selected failed (Error: %d)", Socket::GetLastError());
			return;
		}

		if (FD_ISSET(_socket.GetHandle(), &rfds)) // Main socket
		{
			SocketAddress addr;
			Client* client = new Client();
			client->socket = _socket.Accept(addr);

			addr.ToString(str_addr);
			logging::Info("ConsoleServer: New connection from %s", str_addr.c_str());

			// Find an empty slot
			bool found = false;
			for (uint32_t i = 0; i < _clients.size(); ++i)
			{
				if (_clients[i] == NULL)
				{
					_clients[i] = client;
					found = true;
				}
			}
			if (!found) // If not found, push to the back of the vector
				_clients.push_back(client);

			// Add new socket to fd set for select
			FD_SET(client->socket.GetHandle(), &_fds);
			_fdmax = ((int)client->socket.GetHandle() > _fdmax) ? (int)client->socket.GetHandle() : _fdmax;

			SendHello(client);

		}

		json::Reader reader;
		for (uint32_t i = 0; i < _clients.size(); ++i)
		{
			Client* client = _clients[i];
			if (client == NULL) // Skip empty slots
				continue;
			if (FD_ISSET(client->socket.GetHandle(), &rfds))
			{
				int read_bytes;

				string buffer;
				char c;

				while ((read_bytes = client->socket.Recv(&c, 1)) > 0)
				{
					if (c == '\n')
					{
						// Incomming command
						ConfigValue command;
						if (!reader.Read(buffer.c_str(), buffer.size() + 1, command))
						{
							logging::Warning("ConsoleServer: Failed to parse input: %s", buffer.c_str());
							break;
						}

						ProcessCommand(command);
						buffer.clear();
						break;
					}
					else
					{
						buffer += c;
					}
				}

				if (read_bytes <= 0)
				{
					SocketAddress addr;

					client->socket.GetPeerName(addr);
					addr.ToString(str_addr);

					FD_CLR(client->socket.GetHandle(), &_fds);
					client->socket.Close();

					delete client;
					_clients[i] = NULL;

					// Connection closed
					logging::Info("ConsoleServer: %s disconnected", str_addr.c_str());
				}
			}
		}


	}
	//-------------------------------------------------------------------------------
	void ConsoleServer::Send(const ConfigValue& value)
	{
		json::Writer w;
		stringstream ss;
		w.Write(value, ss, false);

		string msg = ss.str();
		for (auto& client : _clients)
		{
			if (client == NULL) // Skip empty slots
				continue;

			client->socket.Send(msg.c_str(), (int)msg.size());
		}
	}

	//-------------------------------------------------------------------------------

	void ConsoleServer::AddCommand(const char* command, console::CommandCallback callback, void* user_data, const char* description)
	{
		map<string, Command>::iterator it = _commands.find(command);
		if (it != _commands.end()) // Already registered
			return;

		Command cmd;
		cmd.callback = callback;
		cmd.user_data = user_data;
		cmd.description = description;
		_commands[command] = cmd;

		ConfigValue add_cmd_msg; // Notify any clients about the new command
		add_cmd_msg.SetEmptyObject();
		add_cmd_msg["type"].SetString("add_command");
		add_cmd_msg["command"].SetString(command);
		add_cmd_msg["description"].SetString(description);

		Send(add_cmd_msg);
	}
	void ConsoleServer::RemoveCommand(const char* command)
	{
		map<string, Command>::iterator it = _commands.find(command);
		if (it == _commands.end())
			return;

		_commands.erase(it);

		ConfigValue remove_cmd_msg; // Notify any clients about the new command
		remove_cmd_msg.SetEmptyObject();
		remove_cmd_msg["type"].SetString("remove_command");
		remove_cmd_msg["command"].SetString(command);

		Send(remove_cmd_msg);
	}

	//-------------------------------------------------------------------------------

	void ConsoleServer::SendHello(Client* client)
	{
		ConfigValue msg;
		msg.SetEmptyObject();
		msg["type"].SetString("hello");

		char filename[260];
#ifdef SANDBOX_PLATFORM_WIN
		GetModuleFileName(NULL, filename, 260);
#elif SANDBOX_PLATFORM_MACOSX
		printf("proc #%d", getpid());
#endif
		FilePath path(filename);

		msg["title"].SetString(path.Filename().c_str());

		// Also provide any registered commands
		msg["commands"].SetEmptyArray();

		map<string, Command>::iterator it, end;
		it = _commands.begin(); end = _commands.end();
		for (uint32_t i = 0; it != end; ++it, ++i)
		{
			msg["commands"].Append();

			ConfigValue& cmd = msg["commands"][i];
			cmd.SetEmptyObject();
			cmd["command"].SetString(it->first.c_str());
			cmd["description"].SetString(it->second.description.c_str());
		}

		json::Writer w;
		stringstream ss;
		w.Write(msg, ss, false);

		client->socket.Send(ss.str().c_str(), (int)ss.str().size());
	}

	//-------------------------------------------------------------------------------
	void ConsoleServer::ProcessCommand(const ConfigValue& cmd)
	{
		Assert(cmd.IsObject());
		if (!cmd["type"].IsString())
		{
			logging::Warning("ConsoleServer: Invalid command.");
			return;
		}

		if (strcmp(cmd["type"].AsString(), "command") == 0)
		{
			Assert(cmd["command"].IsString());
			map<string, Command>::iterator it = _commands.find(cmd["command"].AsString());
			if (it == _commands.end())
			{
				logging::Warning("Command '%s' not found.", cmd["command"].AsString());
				return;
			}

			it->second.callback(it->second.user_data, cmd);
		}
	}
	//-------------------------------------------------------------------------------


} // namespace sb

