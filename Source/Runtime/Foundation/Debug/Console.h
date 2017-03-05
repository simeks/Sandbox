// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_CONSOLE_H__
#define __FOUNDATION_CONSOLE_H__


#ifdef SANDBOX_DEVELOPMENT
#define ADD_CONSOLE_COMMAND(cmd, callback, user_data, desc) console::AddCommand(cmd, callback, user_data, desc)
#define REMOVE_CONSOLE_COMMAND(cmd) console::RemoveCommand(cmd)
#else
#define ADD_CONSOLE_COMMAND(cmd, callback, user_data, desc) 
#define REMOVE_CONSOLE_COMMAND(cmd) 
#endif

namespace sb
{

	class ConfigValue;
	class ConsoleServer;

	/// Namespace holding console related stuff
	namespace console
	{
		typedef void(*CommandCallback)(void*, const ConfigValue&);

		/// @param port Specifies which port to listen for connections on
		void Initialize(int port);

		/// Shuts down the console
		void Shutdown();

		/// @return True if console is initialized, false if not
		bool Initialized();

		/// Registers a new command callback for when receiving commands from clients
		/// @param command Command name
		/// @sa RemoveCommand
		void AddCommand(const char* command, CommandCallback callback, void* user_data, const char* description);

		/// Unregisters a command registered with AddCommand.
		/// @param command Command name
		/// @sa AddCommand
		void RemoveCommand(const char* command);


		ConsoleServer* Server();

	};

} // namespace sb

#endif // __FOUNDATION_CONSOLE_H__
