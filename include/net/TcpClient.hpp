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

#ifndef TCPCLIENT_HPP_
#define TCPCLIENT_HPP_

#include "../StackTrace.hpp"
#include "../Event.hpp"
#include "../state/StateMachine.hpp"

#include "Socket.hpp"

namespace nitrus {

/**
 * A class that provides functionality for connected socket communication.
 * A tcp client socket must connect to a tcp server socket in order to communicate.
 */
class TcpClient : public Socket {
public:

	/**
	 * How often the socket should check for pending data.
	 */
	static TimeSpan DefaultDataPollFrequency;

	/**
	 * The maximum number of bytes capable of being read at once from a single socket.
	 */
	static size_t DefaultDataBufferSize;

	/**
	 * A class that encapsulates a connection on the socket.
	 */
	class ClientConnectedEventArgs : public EventArgs {
	public:

		/**
		 * Creates a new event argument with the specified data.
		 */
		ClientConnectedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes the event argument.
		 */
		virtual ~ClientConnectedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	typedef EventHandler<const ClientConnectedEventArgs&> ClientConnectedEventHandler;
	typedef Event<const ClientConnectedEventArgs&> ClientConnectedEvent;

	/**
	 * A class that encapsulates a disconnection on the socket.
	 */
	class ClientDisconnectedEventArgs : public EventArgs {
	public:

		/**
		 * Creates a new event argument.
		 */
		ClientDisconnectedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes the event argument.
		 */
		virtual ~ClientDisconnectedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	typedef EventHandler<const ClientDisconnectedEventArgs&> ClientDisconnectedEventHandler;
	typedef Event<const ClientDisconnectedEventArgs&> ClientDisconnectedEvent;

	/**
	 * A class that encapsulates data received from the socket.
	 */
	class DataReceivedEventArgs : public EventArgs {
	private:
		std::string _data;

	public:

		/**
		 * Creates a new event argument with the specified data.
		 *
		 * @param data The data sent to this socket.
		 */
		DataReceivedEventArgs(const std::string& data) : _data(data) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Returns the data sent to this socket.
		 *
		 * @return The data.
		 */
		const std::string& Data() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _data;
		}

		/**
		 * Deletes the event argument.
		 */
		virtual ~DataReceivedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	typedef EventHandler<const DataReceivedEventArgs&> DataReceivedEventHandler;
	typedef Event<const DataReceivedEventArgs&> DataReceivedEvent;

private:
	enum State {
		State_Idle,
		State_Connecting,
		State_Connected,
		State_Sending,
		State_Disconnected
	};

	enum Trigger {
		Trigger_Connect,
		Trigger_Connected,
		Trigger_Send,
		Trigger_Timeout,
		Trigger_Disconnected
	};

private:
	StateMachine<State, Trigger> _stateMachine;
	size_t _bufferSize;
	TimeSpan _poll;
	ClientConnectedEvent _clientConnected;
	ClientDisconnectedEvent _clientDisconnected;
	DataReceivedEvent _dataReceived;
	std::string _sendBuffer;

private:
	template <typename Signature> friend class Delegate;

	/**
	 * Checks if the socket is connected.
	 */
	void Connecting_OnEntry() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (Poll(SelectMode_Write)) {
			_stateMachine.Fire(Trigger_Connected);
		}
		else if (Poll(SelectMode_Error)) {
			_stateMachine.Fire(Trigger_Disconnected);
		}
		else {
			Thread::SetTimeout(_poll, delegate(&TcpClient::Connecting_OnEntry, this));
		}
	}

	/**
	 * Triggers the connected event and starts checking for incoming data.
	 */
	void Connected_OnEntry() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_sendBuffer.clear();
		_clientConnected(ClientConnectedEventArgs(), this);
		Thread::Invoke(delegate(&TcpClient::Connected_Update, this));
	}

	/**
	 * Sends another chunk of queued data.
	 */
	void Sending_OnEntry() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		size_t count = Socket::Send(_sendBuffer);
		_sendBuffer.erase(0, count);

		if (_sendBuffer.empty() == false) {
			_stateMachine.Fire(Trigger_Send);
		}
	}

	/**
	 * Triggers the disconnected event.
	 */
	void Disconnected_OnEntry() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_clientDisconnected(ClientDisconnectedEventArgs(), this);
	}

	/**
	 * Checks for incoming data on the socket.
	 */
	void Connected_Update() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		std::string data;

		if (Poll(SelectMode_Read)) {
			if ((data = Receive(_bufferSize)).empty()) {
				_stateMachine.Fire(Trigger_Disconnected);
			}
			else {
				_dataReceived(DataReceivedEventArgs(data), this);
				Thread::Invoke(delegate(&TcpClient::Connected_Update, this));
			}
		}
		else {
			Thread::SetTimeout(_poll, delegate(&TcpClient::Connected_Update, this));
		}
	}

protected:

	friend class TcpServer;

	/**
	 * Assumes the socket has already been connected as in the case of a client accepted from a tcp server.
	 */
	void AssumeConnected() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_stateMachine.Fire(Trigger_Connected);
	}

public:

	/**
	 * Creates a new connected socket.
	 *
	 * @param bufferSize The maximum number of bytes capable of being received.
	 * @param poll The maximum update interval used to check for incoming data.
	 */
	TcpClient(size_t bufferSize = DefaultDataBufferSize, const TimeSpan& poll = DefaultDataPollFrequency) : Socket(AF_INET, SOCK_STREAM, IPPROTO_TCP), _stateMachine(State_Idle), _bufferSize(bufferSize), _poll(poll), _clientConnected(), _clientDisconnected(), _dataReceived(), _sendBuffer() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_stateMachine.Configure(State_Idle)
			.Permit(Trigger_Connected, State_Connected)
			.Permit(Trigger_Connect, State_Connecting);

		_stateMachine.Configure(State_Connecting)
			.OnEntry(delegate(&TcpClient::Connecting_OnEntry, this))
			.Permit(Trigger_Connected, State_Connected)
			.Permit(Trigger_Disconnected, State_Disconnected)
			.Permit(Trigger_Timeout, State_Disconnected);

		_stateMachine.Configure(State_Connected)
			.OnEntry(delegate(&TcpClient::Connected_OnEntry, this))
			.Permit(Trigger_Send, State_Sending)
			.Permit(Trigger_Disconnected, State_Disconnected);

		_stateMachine.Configure(State_Sending).SubstateOf(State_Connected)
			.OnEntry(delegate(&TcpClient::Sending_OnEntry, this));

		_stateMachine.Configure(State_Disconnected)
			.OnEntry(delegate(&TcpClient::Disconnected_OnEntry, this));

		Block(false);
	}

	/**
	 * The event used to notify listeners when the client has connected.
	 *
	 * @return The event.
	 */
	ClientConnectedEvent& ClientConnected() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _clientConnected;
	}

	/**
	 * The event used to notify listeners when data has been received.
	 *
	 * @return The event.
	 */
	DataReceivedEvent& DataReceived() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _dataReceived;
	}

	/**
	 * The event used to notify listeners when the client has disconnected.
	 *
	 * @return The event.
	 */
	ClientDisconnectedEvent& ClientDisconnected() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _clientDisconnected;
	}

	/**
	 * Connects the socket to the specified endpoint.
	 *
	 * @param endpoint The endpoint to connect to.
	 */
	void Connect(const Endpoint& endpoint) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Socket::Connect(endpoint);
		_stateMachine.Fire(Trigger_Connect);
	}

	/**
	 * Sends data to a connected socket.
	 * If not all data could be sent, the remaining data is queued and another send is attempted later.
	 *
	 * @param value The data to send.
	 */
	void Send(const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_sendBuffer += value;
		_stateMachine.Fire(Trigger_Send);
	}

	/**
	 * Disconnects the socket.
	 */
	void Disconnect() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_stateMachine.Fire(Trigger_Disconnected);
	}

	/**
	 * Deletes the connected socket.
	 */
	virtual ~TcpClient() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if(_stateMachine.State() == State_Connected || _stateMachine.State() == State_Connecting) {
			_stateMachine.Fire(Trigger_Disconnected);
		}
	}
};

TimeSpan TcpClient::DefaultDataPollFrequency = TimeSpan::FromMilliseconds(1);
size_t TcpClient::DefaultDataBufferSize = 4096;

}

#endif /* TCPCLIENT_HPP_ */
