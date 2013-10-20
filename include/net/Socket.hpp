/*
 * Copyright (c) 2012 Christopher M. Baker
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include "../StackTrace.hpp"
#include "../TimeSpan.hpp"
#include "../Thread.hpp"

#ifdef _WIN32
# include <windows.h>
# pragma comment(lib, "ws2_32")
# define sock_handle SOCKET
# define ERR_INPROGRESS WSAEWOULDBLOCK
# define ERR_TRYAGAIN WSAEWOULDBLOCK
# define socklen_t int
# define sock_open     ::socket
# define sock_close    ::closesocket
# define sock_connect  ::connect
# define sock_select   ::select
# define sock_bind     ::bind
# define sock_listen   ::listen
# define sock_accept   ::accept
# define sock_setopt   ::setsockopt
# define sock_ioctl    ::ioctlsocket
# define sock_receive(handle, buffer, size) ::recv(handle, buffer, size, 0)
# define sock_send(handle, buffer, size)    ::send(handle, buffer, size, 0)
# define sock_recvfrom ::recvfrom
# define sock_sendto   ::sendto
# define sock_error    ::WSAGetLastError
#else
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <sys/ioctl.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <errno.h>
# define INVALID_SOCKET (-1)
# define ERR_INPROGRESS EINPROGRESS
# define ERR_TRYAGAIN EAGAIN
# define sock_handle int
# define sock_open     ::socket
# define sock_close    ::close
# define sock_connect  ::connect
# define sock_select   ::select
# define sock_bind     ::bind
# define sock_listen   ::listen
# define sock_accept   ::accept
# define sock_setopt   ::setsockopt
# define sock_ioctl    ::ioctl
# define sock_receive  ::read
# define sock_send     ::write
# define sock_recvfrom ::recvfrom
# define sock_sendto   ::sendto
# define sock_error()  errno
#endif

namespace nitrus {

/**
 * A class that provides functionality for socket communication.
 */
class Socket {
public:

	/**
	 * A class that encapsulates an endpoint for a socket.
	 */
	class Endpoint {
	private:
		std::string _address;
		int _port;

	public:

		/**
		 * Creates a new endpoint with the default address and port.
		 */
		Endpoint() : _address(), _port() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Creates a new endpoint with the specified address and port.
		 *
		 * @param address The address of the endpoint.
		 * @param port The port for the endpoint.
		 */
		Endpoint(const std::string& address, int port) : _address(address), _port(port) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Returns the address associated with the endpoint.
		 *
		 * @return The address.
		 */
		const std::string& Address() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _address;
		}

		/**
		 * Returns the port associated with the endpoint.
		 *
		 * @return The port.
		 */
		int Port() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _port;
		}

		/**
		 * Deletes the endpoint.
		 */
		virtual ~Endpoint() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Prints the endpoint information to the specified stream.
		 *
		 * @param stream The stream to write to.
		 * @param endpoint The endpoint to print.
		 * @return The stream.
		 */
		friend std::ostream& operator << (std::ostream& stream, const Endpoint& endpoint) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return stream << endpoint.Address() << ":" << endpoint.Port();
		}
	};

	/**
	 * An enumeration that defines which file descriptor set to select.
	 */
	enum SelectMode {
		SelectMode_Read,  //!< SelectMode_Read The read file descriptor.
		SelectMode_Write, //!< SelectMode_Write The write file descriptor.
		SelectMode_Error  //!< SelectMode_Error The error file descriptor.
	};

private:
	/**
	 * A class that provides functionality to handle the lifetime of sockets.
	 * For windows systems, this will start and stop window sockets.
	 * For all other systems, this is an empty class.
	 */
	class Lifetime {
	public:

		/**
		 * Creates a new socket lifetime object.
		 */
		Lifetime() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
#ifdef _WIN32
			WSAData data;
			WSAStartup(MAKEWORD(2, 2), &data);
#endif
		}

		/**
		 * Deletes the socket lifetime object.
		 */
		virtual ~Lifetime() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
#ifdef _WIN32
			WSACleanup();
#endif
		}
	};

protected:
	static Lifetime _lifetime;
	sock_handle _handle;
	int _family;

	/**
	 * Sends an input/output control command to the socket.
	 *
	 * @param command The command to send.
	 * @param value The parameter to the command.
	 * @return The result of the command.
	 */
	unsigned long Ioctl(long command, unsigned long value) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		unsigned long buffer = value;

		if (sock_ioctl(_handle, command, &buffer) != 0) {
			throw IoctlException();
		}

		return buffer;
	}

	/**
	 * Resolves the endpoint to a socket address structure.
	 *
	 * @param endpoint The endpoint to resolve.
	 * @return The resolved socket address structure.
	 */
	sockaddr_in Resolve(const Endpoint& endpoint) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		struct hostent* host = gethostbyname(endpoint.Address().c_str());

		if (host == NULL) {
			throw HostNotFoundException();
		}

		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = _family;
		addr.sin_port = htons(endpoint.Port());
		memcpy(&addr.sin_addr, host->h_addr, sizeof(addr.sin_addr));

		return addr;
	}

	/**
	 * Resolves the socket address structure to an endpoint.
	 *
	 * @param address The address to resolve.
	 * @return The resolved endpoint.
	 */
	Endpoint Resolve(const sockaddr_in& address) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return Endpoint(inet_ntoa(address.sin_addr), ntohs(address.sin_port));
	}

	/**
	 * Changes the internal socket handle to another socket handle.
	 * If a valid socket handle was already assigned to this socket then it will be closed.
	 *
	 * @param handle The handle to change to.
	 */
	void SetHandle(sock_handle handle) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_handle != INVALID_SOCKET) {
			sock_close(_handle);
		}

		_handle = handle;
	}

public:

	/**
	 * A class that encapsulates an exception when performing input/output control.
	 */
	class IoctlException : public std::runtime_error {
	public:

		/**
		 * Creates a new input/output control exception.
		 */
		IoctlException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes the input/output control exception.
		 */
		virtual ~IoctlException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that encapsulates an exception when performing a bind.
	 */
	class BindException : public std::runtime_error {
	public:

		/**
		 * Creates a new bind exception.
		 */
		BindException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes the bind exception.
		 */
		virtual ~BindException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that encapsulates an exception when a client connection fails due to a server refusing the connection.
	 */
	class ConnectionRefusedException : public std::runtime_error {
	public:

		/**
		 * Creates a new connection refused exception.
		 */
		ConnectionRefusedException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes the connection refused exception.
		 */
		virtual ~ConnectionRefusedException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that encapsulates an exception when a client connection fails due to the inability to resolve the host name of the server.
	 */
	class HostNotFoundException : public std::runtime_error {
	public:

		/**
		 * Creates a new host not found exception.
		 */
		HostNotFoundException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes the host not found exception.
		 */
		virtual ~HostNotFoundException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that encapsulates an exception when an invalid socket handle is referenced.
	 */
	class InvalidHandleException : public std::runtime_error {
	public:

		/**
		 * Creates a new invalid handle exception.
		 */
		InvalidHandleException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes the invalid handle exception.
		 */
		virtual ~InvalidHandleException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that encapsulates an exception when a send attempt fails.
	 */
	class SendException : public std::runtime_error {
	public:

		/**
		 * Creates a new send exception.
		 */
		SendException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes the send exception.
		 */
		virtual ~SendException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that encapsulates an exception when setting an option fails.
	 */
	class InvalidOptionException : public std::runtime_error {
	public:

		/**
		 * Creates a new invalid option exception.
		 */
		InvalidOptionException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes the invalid option exception.
		 */
		virtual ~InvalidOptionException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * A class that encapsulates an exception when attempting to listen for incoming connections.
	 */
	class ListenException : public std::runtime_error {
	public:

		/**
		 * Creates a new listen exception.
		 */
		ListenException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes the listen exception.
		 */
		virtual ~ListenException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * Creates a new socket with an invalid handle.
	 */
	Socket() : _handle(INVALID_SOCKET), _family(AF_INET) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new socket with the specified address family, socket type, and protocol.
	 *
	 * @param family The address family.
	 * @param type The socket type.
	 * @param protocol The protocol type.
	 */
	Socket(int family, int type, int protocol) : _handle(sock_open(family, type, protocol)), _family(family) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_handle == INVALID_SOCKET) {
			throw InvalidHandleException();
		}
	}

	/**
	 * Binds the socket to the specified port.
	 *
	 * @param port The port to bind to.
	 */
	void Bind(int port) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		struct sockaddr_in addr;

		memset(&addr, 0, sizeof(addr));
		addr.sin_family = _family;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = htonl(INADDR_ANY);

		if (sock_bind(_handle, (sockaddr*) &addr, sizeof(addr)) != 0) {
			throw BindException();
		}
	}

	/**
	 * Connects the socket to the specified endpoint.
	 *
	 * @param endpoint The endpoint to connect to.
	 */
	void Connect(const Endpoint& endpoint) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		struct sockaddr_in addr = Resolve(endpoint);

		if (sock_connect(_handle, (struct sockaddr*) &addr, sizeof(addr)) < 0 && sock_error() != ERR_INPROGRESS) {
			throw ConnectionRefusedException();
		}
	}

	/**
	 * Sets an option on a socket.
	 *
	 * @param level The option level.
	 * @param name The option name.
	 * @param value The option value.
	 * @param length The option length.
	 */
	template <typename T> void SetOption(int level, int name, T value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (sock_setopt(_handle, level, name, (const char*) &value, sizeof(value)) != 0) {
			throw InvalidOptionException();
		}
	}

	/**
	 * Polls the socket to determine the state of a socket select mode.
	 *
	 * @param mode The mode to poll.
	 * @param timeout The maximum time to poll the state.
	 * @return If the mode is SelectMode_Read, true is returned if (1) a client socket is pending, (2) data is available to be read, or (3) the socket has been closed; false otherwise. If the mode is SelectMode_Write, true is returned if (1) a call to Connect has succeeded, or (2) if data can be sent; false otherwise. If the mode is SelectMode_Error, true is returned if a call to Connect has failed; false otherwise.
	 *
	 */
	bool Poll(const SelectMode& mode, const TimeSpan& timeout = TimeSpan::Zero()) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		timeval tv;
		fd_set fdset;

		FD_ZERO(&fdset);
		FD_SET(_handle, &fdset);

		tv.tv_sec = timeout.TotalSeconds();
		tv.tv_usec = timeout.Milliseconds() * 1000;

		if (mode == SelectMode_Read) {
			return sock_select(_handle + 1, &fdset, NULL, NULL, &tv) > 0;
		}
		else if (mode == SelectMode_Write) {
			return sock_select(_handle + 1, NULL, &fdset, NULL, &tv) > 0;
		}
		else if (mode == SelectMode_Error) {
			return sock_select(_handle + 1, NULL, NULL, &fdset, &tv) > 0;
		}

		return false;
	}

	/**
	 * Sets the blocking mode of the socket.
	 *
	 * @param blocking True changes the socket to blocking mode. False changes the socket to non-blocking mode.
	 */
	void Block(bool blocking) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Ioctl(FIONBIO, blocking ? 0 : 1);
	}

	/**
	 * Determines the number of bytes immediately available to be read from the socket.
	 * Before calling this function, the socket should be polled with SelectMode_Read to determine if data is pending.
	 * If data is pending, but the number of available bytes is zero, then the socket has been closed.
	 *
	 * @return The number of bytes available to be read.
	 */
	unsigned long Available() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return Ioctl(FIONREAD, 0);
	}

	/**
	 * Listens for incoming socket connections.
	 *
	 * @param backlog The maximum number of pending connections.
	 */
	void Listen(int backlog = SOMAXCONN) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (sock_listen(_handle, backlog) != 0) {
			throw ListenException();
		}
	}

	/**
	 * Allows a pending socket connection to connect to this socket
	 * This assumes that the socket is listening for incoming socket connections.
	 *
	 * @param child The child socket that connected to this socket.
	 * @param endpoint The endpoint of the child socket.
	 * @return The number of bytes available to be read.
	 */
	bool Accept(Socket& child, Endpoint& endpoint) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		sockaddr_in addr;
		socklen_t size = sizeof(addr);
		sock_handle handle = sock_accept(_handle, (struct sockaddr *) &addr, &size);

		if (handle == INVALID_SOCKET) {
			return false;
		}

		child.SetHandle(handle);
		endpoint = Resolve(addr);
		return true;
	}

	/**
	 * Receives data from a connected socket.
	 * If the socket is in blocking mode, this function will block until the specified number of bytes have been received.
	 * If the socket is in non-blocking mode, this function will return immediately but may not read all of the bytes requested.
	 * If not all data could be received, the length of the string returned will be less than the specified count.
	 *
	 * @param count The number of bytes to receive.
	 * @return The data received.
	 */
	std::string Receive(size_t count) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		int bytesReceived;
		std::vector<char> buffer(count);

		if ((bytesReceived = sock_receive(_handle, &buffer[0], count)) < 0) {
			return std::string();
		}

		return std::string(&buffer[0], bytesReceived);
	}

	/**
	 * Receives data from an unconnected socket.
	 * If the socket is in blocking mode, this function will block until the specified number of bytes have been received.
	 * If the socket is in non-blocking mode, this function will return immediately but may not read all of the bytes requested.
	 * If not all data could be received, the length of the string returned will be less than the specified count.
	 *
	 * @param endpoint The endpoint of the socket that sent data to this socket.
	 * @param count The number of bytes to receive.
	 * @return The data received.
	 */
	std::string Receive(Endpoint& endpoint, int count) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		struct sockaddr_in addr;
		socklen_t addrLength = sizeof(addr);
		int bytesReceived;
		std::vector<char> buffer(count);

		if ((bytesReceived = sock_recvfrom(_handle, &buffer[0], count, 0, (sockaddr*) &addr, &addrLength)) < 0) {
			endpoint = Endpoint();
			return std::string();
		}

		endpoint = Endpoint(inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
		return std::string(&buffer[0], bytesReceived);
	}

	/**
	 * Sends data to a connected socket.
	 * If the socket is in blocking mode, this function will block until the specified number of bytes has been sent.
	 * If the socket is in non-blocking mode, this function will return immediately but may not have sent all of the bytes requested.
	 * If not all data could be sent, the value returned will be less than the length of the data.
	 *
	 * @param value The data to send.
	 * @return The number of bytes sent.
	 */
	size_t Send(const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		int bytesSent;

		if ((bytesSent = sock_send(_handle, value.data(), value.size())) < 0) {
			if (sock_error() == ERR_INPROGRESS || sock_error() == ERR_TRYAGAIN) {
				return 0;
			}
			else {
				throw SendException();
			}
		}

		return bytesSent;
	}

	/**
	 * Sends data to an unconnected socket.
	 * If the socket is in blocking mode, this function will block until the specified number of bytes has been sent.
	 * If the socket is in non-blocking mode, this function will return immediately but may not have sent all of the bytes requested.
	 * If not all data could be sent, the value returned will be less than the length of the data.
	 *
	 * @param endpoint The endpoint of the socket to send data to.
	 * @param value The data to send.
	 * @return The number of bytes sent.
	 */
	size_t Send(const Endpoint& endpoint, const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		int bytesSent;
		struct sockaddr_in addr = Resolve(endpoint);

		if ((bytesSent = sock_sendto(_handle, value.data(), value.size(), 0, (sockaddr*) &addr, sizeof(addr))) < 0) {
			if (sock_error() == ERR_INPROGRESS || sock_error() == ERR_TRYAGAIN) {
				return 0;
			}
			else {
				throw SendException();
			}
		}

		return bytesSent;
	}

	/**
	 * Deletes the socket.
	 */
	virtual ~Socket() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_handle != INVALID_SOCKET) {
			sock_close(_handle);
		}
	}
};

Socket::Lifetime Socket::_lifetime = Socket::Lifetime();

}

#endif /* SOCKET_HPP_ */
