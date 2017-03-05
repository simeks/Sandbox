// Copyright 2008-2014 Simon Ekström

#ifndef __FOUNDATION_SOCKET_H__
#define __FOUNDATION_SOCKET_H__

#ifdef SANDBOX_PLATFORM_WIN
#include <WinSock2.h>

typedef int socklen_t;

#elif SANDBOX_PLATFORM_MACOSX
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>


#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#endif


namespace sb
{

	class IPAddress
	{
	public:
		static const uint32_t ANY_ADDR = 0;

		/// Checks if the specified string is a valid address, string can be either
		///		an ip or a hostname.
		static bool IsValid(const char* str);

	public:
		/// Construct from a string, e.g. "127.0.0.1" or "google.com"
		IPAddress(const char* addr);
		IPAddress(struct in_addr addr);
		IPAddress();

		void ToString(string& str) const;
		struct in_addr ToInAddr() const;

		bool operator==(const IPAddress& other) const;
		bool operator!=(const IPAddress& other) const;

	private:
		uint32_t _addr;

	};

	class SocketAddress
	{
	public:
		static const int ANY_PORT = 0;

		/// Checks if the specified string is a valid address, string can be either
		///		an ip or a hostname with an following port number (E.g. google.com:80).
		static bool IsValid(const char* str);

	public:
		SocketAddress(const IPAddress& addr, int port);
		/// Construct from a string, e.g. "127.0.0.1:80" or "google.com:80"
		SocketAddress(const char* str);
		SocketAddress(sockaddr_in addr);
		SocketAddress();

		void ToString(string& str) const;
		struct sockaddr_in ToSockAddr() const;

		const IPAddress& Address() const;
		int Port() const;

		bool operator==(const SocketAddress& other) const;
		bool operator!=(const SocketAddress& other) const;

	private:
		IPAddress _addr;
		int _port;
	};


	class Socket
	{
	public:
#ifdef SANDBOX_PLATFORM_WIN
		typedef SOCKET Handle;
#elif SANDBOX_PLATFORM_MACOSX
		typedef int Handle;
#endif

		enum SocketType
		{
			TCP,
			UDP
		};

		static int Select(uint32_t nfs, fd_set* rfds, fd_set* wfds, float timeout);
		static int GetLastError();

	public:
		Socket();
		~Socket();

		/// Creates the socket
		void Create(SocketType type);
		/// Closes the socket
		void Close();

		/// @return True if socket is valid, else false
		bool Valid() const;
		Handle GetHandle() const;

		int GetPeerName(SocketAddress& addr) const;

		/// Sets whetever this socket should be in blocking mode
		void SetBlocking(bool block);
		/// Enables/disables broadcast for UDP sockets, does nothing for a TCP socket
		void SetBroadcast(bool broadcast);


		/// @brief Tries to bind the socket to the specified address
		bool Bind(const SocketAddress& addr);

		/// @brief Functions for connection based protocols, meaning they are invalid for UDP.
		/// @{

		/// @brief Accepts an incoming connection
		Socket Accept(SocketAddress& remote_addr);

		/// @brief Tries to connect to the specified address
		/// @return True if connection is successfully established, else false
		bool Connect(SocketAddress& remote_addr);

		/// @brief Tries to starts to listen
		/// @param backlog Maximum number of pending connections
		/// @return True if successful, else false
		bool Listen(int backlog = SOMAXCONN);

		/// @}

		int Send(const char* buf, int len);
		int SendTo(const char* buf, int len, SocketAddress& addr);

		int Recv(char* buf, int len);
		int RecvFrom(char* buf, int len, SocketAddress& addr);

		bool operator==(const Socket& other) const;
		bool operator!=(const Socket& other) const;


	private:
		static void InitalizeSockets();
		static void ShutdownSockets();

		Socket(Handle socket);

		Handle _socket;
	};

} // namespace sb



#endif // __FOUNDATION_SOCKET_H__

