// Copyright 2008-2014 Simon Ekström

#include "Common.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS // TODO: Fix use of deprecated functions

#include "Socket.h"

namespace sb
{

	//-----------------------------------------------------------------------------
	bool IPAddress::IsValid(const char* str)
	{
		unsigned long addr = inet_addr(str);

		if (addr == INADDR_NONE)
		{
			struct hostent* host = gethostbyname(str);
			if (host == NULL)
			{
				// Host couldn't be found
				return false;
			}
			else
			{
				// Host found
				return true;
			}
		}

		return true;
	}
	//-------------------------------------------------------------------------------
	IPAddress::IPAddress(const char* addr)
	{
		Assert(addr);

		_addr = inet_addr(addr);

		if (_addr == INADDR_NONE)
		{
			struct hostent* host = gethostbyname(addr);
			if (host == NULL)
			{
				// Failed to find host, print warning and set to ANY_ADDR
				int error = Socket::GetLastError();
#ifdef SANDBOX_PLATFORM_WIN
				if (error == WSAHOST_NOT_FOUND)
				{
					logging::Warning("Host '%s' not found", addr);
				}
				else
#endif
				{
					logging::Warning("Failed to resolve '%s', error code: %d", addr, error);
				}
				_addr = ANY_ADDR;
			}
			else
			{
				_addr = *(uint32_t*)host->h_addr_list[0];
			}
		}

	}
	IPAddress::IPAddress(struct in_addr addr)
	{
#ifdef SANDBOX_PLATFORM_WIN
		_addr = addr.S_un.S_addr;
#elif SANDBOX_PLATFORM_MACOSX
		_addr = addr.s_addr;
#endif
	}
	IPAddress::IPAddress()
	{
		_addr = ANY_ADDR;
	}
	void IPAddress::ToString(string& str) const
	{
		in_addr addr;
#ifdef SANDBOX_PLATFORM_WIN
		addr.S_un.S_addr = _addr;
#elif SANDBOX_PLATFORM_MACOSX
		addr.s_addr = _addr;
#endif

		str = inet_ntoa(addr);
	}
	struct in_addr IPAddress::ToInAddr() const
	{
		in_addr addr;

#ifdef SANDBOX_PLATFORM_WIN
		addr.S_un.S_addr = _addr;
#elif SANDBOX_PLATFORM_MACOSX
		addr.s_addr = _addr;
#endif

		return addr;
	}
	bool IPAddress::operator==(const IPAddress& other) const
	{
		return (_addr == other._addr);
	}
	bool IPAddress::operator!=(const IPAddress& other) const
	{
		return (_addr != other._addr);
	}
	//-------------------------------------------------------------------------------


	bool SocketAddress::IsValid(const char* str)
	{
		char addr_str[256];
		strcpy_s(addr_str, str);

		// Check if port is specified in addr (E.g. 127.0.0.1:1111)
		char* port_str = strchr(addr_str, ':');
		if (port_str)
		{
			// Separate address from port, now addr_str is the address and port_str is the port
			*port_str++ = '\0';
		}
		return IPAddress::IsValid(addr_str);
	}
	//-------------------------------------------------------------------------------

	SocketAddress::SocketAddress(const IPAddress& addr, int port)
	{
		_addr = addr;
		_port = port;
	}
	SocketAddress::SocketAddress(const char* str)
	{
		char addr_str[256];
		strcpy_s(addr_str, str);

		// Check if port is specified in addr (E.g. 127.0.0.1:1111)
		char* port_str = strchr(addr_str, ':');
		if (port_str)
		{
			// Separate address from port, now addr_str is the address and port_str is the port
			*port_str++ = '\0';
			_port = atoi(port_str);
		}
		else
		{
			_port = ANY_PORT;
		}

		_addr = IPAddress(addr_str);

	}
	SocketAddress::SocketAddress(sockaddr_in addr)
	{
		_addr = IPAddress(addr.sin_addr);
		_port = htons(addr.sin_port);
	}
	SocketAddress::SocketAddress()
	{
		_port = ANY_PORT;
	}
	void SocketAddress::ToString(string& str) const
	{
		in_addr addr = _addr.ToInAddr();

		stringstream ss; ss << inet_ntoa(addr) << ":" << _port;
		str = ss.str();
	}
	struct sockaddr_in SocketAddress::ToSockAddr() const
	{
		sockaddr_in ret;
		ret.sin_addr = _addr.ToInAddr();
		ret.sin_family = AF_INET;
		ret.sin_port = htons((u_short)_port);

		return ret;
	}
	const IPAddress& SocketAddress::Address() const
	{
		return _addr;
	}
	int SocketAddress::Port() const
	{
		return _port;
	}

	bool SocketAddress::operator==(const SocketAddress& other) const
	{
		return ((_addr == other._addr) && (_port == other._port));
	}
	bool SocketAddress::operator!=(const SocketAddress& other) const
	{
		return ((_addr != other._addr) || (_port != other._port));
	}

	//-------------------------------------------------------------------------------


	int Socket::Select(uint32_t nfs, fd_set* rfds, fd_set* wfds, float timeout)
	{
		timeval t;
		t.tv_sec = (int)timeout;
		t.tv_usec = ((int)timeout - t.tv_sec) * 1000000;

		return select(nfs, rfds, wfds, 0, &t);
	}

	//-------------------------------------------------------------------------------

	Socket::Socket() : _socket(INVALID_SOCKET)
	{
		Socket::InitalizeSockets();
	}
	Socket::Socket(Handle socket)
	{
		Socket::InitalizeSockets();
		_socket = socket;
	}
	Socket::~Socket()
	{
		Socket::ShutdownSockets();
	}
	//-------------------------------------------------------------------------------
	void Socket::Create(SocketType type)
	{
		if (type == TCP)
		{
			_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		}
		else if (type == UDP)
		{
			_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		}
	}
	void Socket::Close()
	{
#ifdef SANDBOX_PLATFORM_WIN
		shutdown(_socket, SD_BOTH);
		closesocket(_socket);
#elif SANDBOX_PLATFORM_MACOSX
		shutdown(_socket, SHUT_RDWR);
		close(_socket);
#endif
		_socket = INVALID_SOCKET;
	}
	//-------------------------------------------------------------------------------
	bool Socket::Valid() const
	{
		return (_socket != INVALID_SOCKET);
	}
	Socket::Handle Socket::GetHandle() const
	{
		return _socket;
	}
	int Socket::GetPeerName(SocketAddress& addr) const
	{
		sockaddr_in sa;
		socklen_t len = sizeof(sockaddr_in);
		int ret = getpeername(_socket, (struct sockaddr*)&sa, &len);
		addr = SocketAddress(IPAddress(sa.sin_addr), htons(sa.sin_port));
		return ret;
	}
	//-------------------------------------------------------------------------------
	void Socket::SetBlocking(bool block)
	{
#ifdef SANDBOX_PLATFORM_WIN
		u_long notblock = block ? 0 : 1;
		if (ioctlsocket(_socket, FIONBIO, &notblock) == SOCKET_ERROR)
		{
			logging::Warning("Failed to set blocking mode for socket (Error: %d)", Socket::GetLastError());
		}
#elif SANDBOX_PLATFORM_MACOSX
		int flags = fcntl(_socket, F_GETFL, 0);
		if (flags < 0)
		{
			logging::Warning("Failed to set blocking mode for socket, F_GETFL failed (Error: %d)", Socket::GetLastError());
			return;
		}

		flags = block ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
		if (fcntl(_socket, F_SETFL, flags) != 0)
		{
			logging::Warning("Failed to set blocking mode for socket, F_SETFL failed (Error: %d)", Socket::GetLastError());
			return;
		}
#endif
	}
	void Socket::SetBroadcast(bool broadcast)
	{
		int bc = broadcast;
		if (setsockopt(_socket, SOL_SOCKET, SO_BROADCAST, (char*)&bc, sizeof(bc)) == SOCKET_ERROR)
		{
			logging::Warning("Failed to set broadcast for socket (Error: %d)", Socket::GetLastError());
		}
	}
	//-------------------------------------------------------------------------------
	bool Socket::Bind(const SocketAddress& addr)
	{
		Assert(_socket != INVALID_SOCKET);
		sockaddr_in bind_addr = addr.ToSockAddr();

		if (bind(_socket, (sockaddr*)&bind_addr, sizeof(sockaddr_in)) == SOCKET_ERROR)
		{
			return false;
		}
		return true;
	}
	Socket Socket::Accept(SocketAddress& remote_addr)
	{
		Assert(_socket != INVALID_SOCKET);

		sockaddr_in ra;
		socklen_t len = sizeof(sockaddr_in);
		Handle socket = accept(_socket, (sockaddr*)&ra, &len);

		remote_addr = SocketAddress(ra);

		return Socket(socket);
	}
	bool Socket::Connect(SocketAddress& remote_addr)
	{
		Assert(_socket != INVALID_SOCKET);

		sockaddr_in addr = remote_addr.ToSockAddr();
		if (connect(_socket, (sockaddr*)&addr, sizeof(sockaddr_in)) == SOCKET_ERROR)
			return false;

		return true;
	}
	bool Socket::Listen(int backlog)
	{
		Assert(_socket != INVALID_SOCKET);
		if (listen(_socket, backlog) == SOCKET_ERROR)
		{
			return false;
		}
		return true;
	}
	//-------------------------------------------------------------------------------
	int Socket::Send(const char* buf, int len)
	{
		Assert(_socket != INVALID_SOCKET);
		return send(_socket, buf, len, 0);
	}
	int Socket::SendTo(const char* buf, int len, SocketAddress& addr)
	{
		Assert(_socket != INVALID_SOCKET);
		sockaddr_in toaddr = addr.ToSockAddr();
		return sendto(_socket, buf, len, 0, (sockaddr*)&toaddr, sizeof(sockaddr_in));
	}

	int Socket::Recv(char* buf, int len)
	{
		Assert(_socket != INVALID_SOCKET);
		return recv(_socket, buf, len, 0);
	}
	int Socket::RecvFrom(char* buf, int len, SocketAddress& addr)
	{
		Assert(_socket != INVALID_SOCKET);
		sockaddr_in fromaddr = addr.ToSockAddr();
		socklen_t fromlen = sizeof(sockaddr_in);
		return recvfrom(_socket, buf, len, 0, (sockaddr*)&fromaddr, &fromlen);
	}
	//-------------------------------------------------------------------------------
	bool Socket::operator==(const Socket& other) const
	{
		return (_socket == other._socket);
	}
	bool Socket::operator!=(const Socket& other) const
	{
		return (_socket != other._socket);
	}
	//-------------------------------------------------------------------------------

} // namespace sb

