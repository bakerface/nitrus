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

#ifndef UDPCLIENT_HPP_
#define UDPCLIENT_HPP_

#include "../StackTrace.hpp"
#include "../Event.hpp"

#include "Socket.hpp"

namespace nitrus {

/**
 * A class that provides functionality for unconnected socket communication.
 */
class UdpClient : public Socket {
public:

	/**
	 * How often the socket should check for pending client data.
	 */
	static TimeSpan DefaultDataPollFrequency;

	/**
	 * The maximum number of bytes capable of being read at once from a single socket.
	 */
	static size_t DefaultDataBufferSize;

	/**
	 * A class that encapsulates data received from the socket.
	 */
	class DataReceivedEventArgs : public EventArgs {
	private:
		Endpoint _sender;
		std::string _data;

	public:

		/**
		 * Creates a new event argument with the specified endpoint and data.
		 *
		 * @param sender The endpoint of the socket that sent data to this socket.
		 * @param data The data sent to this socket.
		 */
		DataReceivedEventArgs(const Endpoint& sender, const std::string& data) : _sender(sender), _data(data) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Returns the endpoint of the socket that sent data to this socket.
		 *
		 * @return The endpoint.
		 */
		const Endpoint& Sender() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _sender;
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
	size_t _bufferSize;
	TimeSpan _poll;
	DataReceivedEvent _dataReceived;

private:
	template <typename Signature> friend class Delegate;

	/**
	 * Checks for data on the socket.
	 */
	void Update() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Endpoint endpoint;
		std::string data;

		if (Poll(SelectMode_Read)) {
			if ((data = Receive(endpoint, _bufferSize)).empty() == false) {
				_dataReceived(DataReceivedEventArgs(endpoint, data), this);
				Thread::Invoke(delegate(&UdpClient::Update, this));
			}
		}
		else {
			Thread::SetTimeout(_poll, delegate(&UdpClient::Update, this));
		}
	}

public:

	/**
	 * Creates a new unconnected socket.
	 *
	 * @param bufferSize The maximum number of bytes capable of being received.
	 * @param poll The maximum update interval used to check for incoming data.
	 */
	UdpClient(size_t bufferSize = DefaultDataBufferSize, const TimeSpan& poll = DefaultDataPollFrequency) : Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP), _bufferSize(bufferSize), _poll(poll), _dataReceived() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		// set the socket into non-blocking mode since we are polling for data
		Block(false);

		// start checking for data at the specified poll interval
		Thread::SetTimeout(_poll, delegate(&UdpClient::Update, this));
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
	 * Deletes the unconnected socket.
	 */
	virtual ~UdpClient() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}
};

TimeSpan TcpClient::DefaultDataPollFrequency = TimeSpan::FromMilliseconds(1);
size_t TcpClient::DefaultDataBufferSize = 1024;

}

#endif /* UDPCLIENT_HPP_ */
