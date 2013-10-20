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

#ifndef TCPSERVER_HPP_
#define TCPSERVER_HPP_

#include "../StackTrace.hpp"
#include "../Event.hpp"
#include "../state/StateMachine.hpp"

#include "Socket.hpp"
#include "TcpClient.hpp"

namespace nitrus {

/**
 * A class that provides functionality for connected socket communication.
 * A tcp server socket waits for a tcp client socket to connect in order to communicate.
 */
class TcpServer : public Socket {
public:

	/**
	 * How often the server should check for pending client connections.
	 */
	static TimeSpan DefaultAcceptPollFrequency;

	/**
	 * A class that encapsulates a pending connection to the socket.
	 */
	class ClientAcceptedEventArgs : public EventArgs {
	private:
		TcpClient* _client;
		Socket::Endpoint _endpoint;

	public:

		/**
		 * Creates a new event argument.
		 *
		 * @param client The client that has been accepted.
		 * @param endpoint The endpoint of the client that has been accepted.
		 */
		ClientAcceptedEventArgs(TcpClient* client, const Socket::Endpoint& endpoint) : _client(client), _endpoint(endpoint) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Creates a new event argument from another event argument.
		 *
		 * @param that The event arguments to copy.
		 */
		ClientAcceptedEventArgs(const ClientAcceptedEventArgs& that) : _client(that._client), _endpoint(that._endpoint) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Returns the client that has been accepted.
		 */
		TcpClient* Client() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _client;
		}

		/**
		 * Returns the endpoint of the client that has been accepted.
		 */
		Socket::Endpoint Endpoint() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _endpoint;
		}

		/**
		 * Clones an event argument into this event argument.
		 *
		 * @param that The event arguments to copy.
		 */
		ClientAcceptedEventArgs& operator = (const ClientAcceptedEventArgs& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_client = that._client;
			_endpoint = that._endpoint;

			return *this;
		}

		/**
		 * Deletes the event argument.
		 */
		virtual ~ClientAcceptedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	typedef EventHandler<const ClientAcceptedEventArgs&> ClientAcceptedEventHandler;
	typedef Event<const ClientAcceptedEventArgs&> ClientAcceptedEvent;

private:
	TimeSpan _poll;
	ClientAcceptedEvent _clientAccepted;

private:

	template <typename Signature> friend class Delegate;

	/**
	 * Called whenever a tcp client that was accepted by this tcp server has been disconnected.
	 * This will just delete the object that was allocated in the Update function.
	 *
	 * @param args The event arguments.
	 * @param sender The tcp client that sent this event.
	 */
	void OnClientDisconnected(const TcpClient::ClientDisconnectedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		TcpClient* client = static_cast<TcpClient*>(sender);
		delete client;
	}

	/**
	 * Checks for pending client connections to this socket.
	 */
	void Update() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (Poll(SelectMode_Read)) {
			Socket::Endpoint endpoint;
			TcpClient* client = new TcpClient();

			if (Socket::Accept(*client, endpoint)) {
				client->Block(false);
				_clientAccepted(ClientAcceptedEventArgs(client, endpoint), this);
				client->ClientDisconnected() += delegate(&TcpServer::OnClientDisconnected, this);

				// this method is a little gross.
				// need to find another way to do this.
				// make sure this is called after the event to allow event handlers to be registered.
				client->AssumeConnected();
			}
			else {
				delete client;
				Log::Warning("A client was pending for the tcp server but it could not be accepted.");
			}
		}

		Thread::SetTimeout(_poll, delegate(&TcpServer::Update, this));
	}

public:

	/**
	 * Creates a new connected socket.
	 *
	 * @param bufferSize The maximum number of bytes capable of being received.
	 * @param poll The maximum update interval used to check for incoming data.
	 */
	TcpServer(const TimeSpan& poll = DefaultAcceptPollFrequency) : Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP), _poll(poll), _clientAccepted() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Block(false);
	}

	/**
	 * The event used to notify listeners when a client is accepted.
	 *
	 * @return The event.
	 */
	ClientAcceptedEvent& ClientAccepted() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _clientAccepted;
	}

	/**
	 * Listens for incoming socket connections.
	 *
	 * @param backlog The maximum number of pending connections.
	 */
	void Listen(int backlog = SOMAXCONN) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Socket::Listen(backlog);
		Thread::SetTimeout(_poll, delegate(&TcpServer::Update, this));
	}

	/**
	 * Deletes the connected socket.
	 */
	virtual ~TcpServer() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}
};

TimeSpan TcpServer::DefaultAcceptPollFrequency = TimeSpan::FromMilliseconds(1);

}

#endif /* TCPSERVER_HPP_ */
