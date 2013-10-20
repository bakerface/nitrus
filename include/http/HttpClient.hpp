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

#ifndef HTTPCLIENT_HPP_
#define HTTPCLIENT_HPP_

#include "../StackTrace.hpp"
#include "../state/StateMachine.hpp"
#include "../net/TcpClient.hpp"

namespace nitrus {

/**
 * A class that provides functionality for connecting to servers over the hypertext transfer protocol.
 * The socket communication provided by this class is insecure; therefore, any sensitive data should be handled by an HttpsClient instead.
 */
class HttpClient : public TcpClient {
public:

	/**
	 * A class that encapsulates the start of an HTTP response.
	 */
	class ResponseStartedEventArgs : public EventArgs {
	private:
		std::string _protocol;
		int _code;
		std::string _description;

	public:

		/**
		 * Creates a new event argument.
		 *
		 * @param protocol The protocol.
		 * @param code The code.
		 * @param description The description.
		 */
		ResponseStartedEventArgs(const std::string& protocol, int code, const std::string& description) : _protocol(protocol), _code(code), _description(description) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Returns the protocol for the response.
		 *
		 * @return The protocol.
		 */
		const std::string& Protocol() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _protocol;
		}

		/**
		 * Returns the response code.
		 *
		 * @return The code.
		 */
		int Code() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _code;
		}

		/**
		 * Returns the description of the response.
		 *
		 * @return The description.
		 */
		const std::string& Description() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _description;
		}

		/**
		 * Deletes this event argument.
		 */
		virtual ~ResponseStartedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	typedef EventHandler<const ResponseStartedEventArgs&> ResponseStartedEventHandler;
	typedef Event<const ResponseStartedEventArgs&> ResponseStartedEvent;

	/**
	 * A class that encapsulates a header for an HTTP response.
	 */
	class HeaderReceivedEventArgs : public EventArgs {
	private:
		std::string _key;
		std::string _value;

	public:

		/**
		 * Creates a new event argument.
		 *
		 * @param key The header key.
		 * @param value The header value.
		 */
		HeaderReceivedEventArgs(const std::string& key, const std::string& value) : _key(key), _value(value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Returns the key for the header.
		 *
		 * @return The key.
		 */
		const std::string& Key() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _key;
		}

		/**
		 * Returns the value for the header.
		 *
		 * @return The value.
		 */
		const std::string& Value() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _value;
		}

		/**
		 * Deletes this event argument.
		 */
		virtual ~HeaderReceivedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	typedef EventHandler<const HeaderReceivedEventArgs&> HeaderReceivedEventHandler;
	typedef Event<const HeaderReceivedEventArgs&> HeaderReceivedEvent;

	/**
	 * A class that encapsulates content for an HTTP response.
	 * This content may be the complete content or may be a partial chunk of the content.
	 */
	class ContentReceivedEventArgs : public EventArgs {
	private:
		std::string _content;

	public:

		/**
		 * Creates a new event argument.
		 *
		 * @param content The content.
		 */
		ContentReceivedEventArgs(const std::string& content) : _content(content) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * The partial content of the response.
		 *
		 * @return The content.
		 */
		const std::string& Content() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _content;
		}

		/**
		 * Deletes this event argument.
		 */
		virtual ~ContentReceivedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	typedef EventHandler<const ContentReceivedEventArgs&> ContentReceivedEventHandler;
	typedef Event<const ContentReceivedEventArgs&> ContentReceivedEvent;

	/**
	 * A class that encapsulates the end of an HTTP response.
	 */
	class ResponseEndedEventArgs : public EventArgs {
	public:

		/**
		 * Creates a new event argument.
		 */
		ResponseEndedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Deletes this event argument.
		 */
		virtual ~ResponseEndedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	typedef EventHandler<const ResponseEndedEventArgs&> ResponseEndedEventHandler;
	typedef Event<const ResponseEndedEventArgs&> ResponseEndedEvent;

private:
	enum State {
		State_WaitForConnection,
		State_Connected,
		State_RequestActionLine,
		State_RequestHeaderLine,
		State_RequestLastHeader,
		State_RequestChunk,
		State_ResponseActionLine,
		State_ResponseHeaderLine,
		State_ResponseHeaderLineAndTransferEncodingChunked,
		State_ResponseHeaderLineAndContentLength,
		State_ResponseHeaderLineConnectionClose,
		State_ResponseHeaderLineAndContentLengthAndConnectionClose,
		State_ResponseHeaderLineAndTransferEncodingChunkedAndConnectionClose,
		State_ResponseContent,
		State_ResponseContentUntilClosed,
		State_ResponseChunkSize,
		State_ResponseChunkSizeAndConnectionClose,
		State_ResponseChunk,
		State_ResponseChunkAndConnectionClose,
		State_EndOfResponseContentUntilClosed,
		State_EndOfResponse,
		State_WaitForDisconnect
	};

	enum Trigger {
		Trigger_Connected,
		Trigger_RequestBegin,
		Trigger_RequestHeader,
		Trigger_RequestChunk,
		Trigger_Continue,
		Trigger_Break,
		Trigger_RequestEnd,
		Trigger_TransferEncodingChunked,
		Trigger_ContentLength,
		Trigger_ConnectionClose,
		Trigger_EndOfChunks,
		Trigger_Disconnect
	};

	StateMachine<State, Trigger> _stateMachine;
	std::string _buffer;
	size_t _contentLength;
	ResponseStartedEvent _responseStarted;
	HeaderReceivedEvent _headerReceived;
	ContentReceivedEvent _contentReceived;
	ResponseEndedEvent _responseEnded;

private:
	template <typename Signature> friend class Delegate;

	/**
	 * Called when the tcp client has connected to the server.
	 *
	 * @param args The event arguments.
	 * @param sender The sender of the event.
	 */
	void OnClientConnected(const TcpClient::ClientConnectedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_stateMachine.Fire(Trigger_Connected);
	}

	/**
	 * Called when the tcp client has disconnected from the server.
	 *
	 * @param args The event arguments.
	 * @param sender The sender of the event.
	 */
	void OnClientDisconnected(const TcpClient::ClientDisconnectedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_stateMachine.Fire(Trigger_Disconnect);
	}

	/**
	 * Called when data is read from the tcp client.
	 * This will progress through the state machine and trigger events.
	 *
	 * @param args The event arguments.
	 * @param sender The sender of the event.
	 */
	void OnDataReceived(const TcpClient::DataReceivedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_buffer += args.Data();
		_stateMachine.Fire(Trigger_Continue);
	}

	/**
	 * Called when the tcp client is waiting to be connected to a server.
	 *
	 * @param args The event arguments.
	 * @param sender The sender of the event.
	 */
	void OnWaitForConnectionEntered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_buffer.clear();
	}

	/**
	 * Called when the last header has been sent for an http request.
	 * This will send another header specifying chunked transfer encoding.
	 */
	void OnLastHeaderEntered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		TcpClient::Send("Transfer-Encoding: chunked\r\n\r\n");
		_stateMachine.Fire(Trigger_Break);
	}

	/**
	 * Called when the action line should attempt to be parsed.
	 * If the action line is successfully parsed, the state machine moves to the next state.
	 * If partial data was received, the state machine waits for more data.
	 */
	void ActionLineEntered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		size_t endOfProtocol, endOfCode, endOfDescription;

		if ((endOfProtocol = _buffer.find(" ")) == std::string::npos) {
			return;
		}
		else if ((endOfCode = _buffer.find(" ", endOfProtocol + 1)) == std::string::npos) {
			return;
		}
		else if ((endOfDescription = _buffer.find("\r\n",  endOfCode + 1)) == std::string::npos) {
			return;
		}

		std::string protocol = _buffer.substr(0, endOfProtocol);
		int code = String::Convert<int>(_buffer.substr(endOfProtocol + 1, endOfCode - endOfProtocol - 1));
		std::string description = _buffer.substr(endOfCode + 1, endOfDescription - endOfCode - 1);
		_buffer.erase(0, endOfDescription + 2);

		_contentLength = 0;
		_responseStarted(ResponseStartedEventArgs(protocol, code, description), this);
		_stateMachine.Fire(Trigger_Break);
	}

	/**
	 * Called when a header line should attempt to be parsed.
	 * If the header line is successfully parsed, the state machine moves to the next state.
	 * If partial data was received, the state machine waits for more data.
	 */
	void HeaderLineEntered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		size_t endOfKey, endOfValue;

		if ((endOfValue = _buffer.find("\r\n")) == std::string::npos) {
			return;
		}
		else if (endOfValue == 0) {
			_buffer.erase(0, 2);
			_stateMachine.Fire(Trigger_Break);
		}
		else if ((endOfKey = _buffer.find(":")) == std::string::npos) {
			return;
		}
		else {
			std::string key = _buffer.substr(0, endOfKey);
			std::string value = _buffer.substr(endOfKey + 2, endOfValue - endOfKey - 2);
			_buffer.erase(0, endOfValue + 2);

			_headerReceived(HeaderReceivedEventArgs(key, value), this);

			if (String::ToLowerCase(key) == "transfer-encoding" && String::ToLowerCase(value) == "chunked") {
				_stateMachine.Fire(Trigger_TransferEncodingChunked);
			}
			else if (String::ToLowerCase(key) == "content-length") {
				_contentLength = String::Convert<size_t>(value);
				_stateMachine.Fire(Trigger_ContentLength);
			}
			else if (String::ToLowerCase(key) == "connection" && String::ToLowerCase(value) == "close") {
				_stateMachine.Fire(Trigger_ConnectionClose);
			}
			else {
				_stateMachine.Fire(Trigger_Continue);
			}
		}
	}

	/**
	 * Called when content should attempt to be received.
	 * This will wait until the socket has been closed before transitioning states.
	 * If partial data was received, the state machine waits for more data.
	 */
	void ContentUntilClosedEntered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_buffer.empty() == false) {
			_contentReceived(ContentReceivedEventArgs(_buffer), this);
			_buffer.clear();
			_stateMachine.Fire(Trigger_Continue);
		}
	}

	/**
	 * Called when content should attempt to be received.
	 * If partial data was received, the state machine waits for more data.
	 */
	void ContentEntered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_contentLength == 0) {
			_stateMachine.Fire(Trigger_Break);
		}
		else if (_buffer.empty() == false) {
			size_t count = _buffer.size();

			if (_contentLength < count) {
				count = _contentLength;
			}

			std::string chunk = _buffer.substr(0, count);
			_buffer.erase(0, count);
			_contentLength -= count;

			_contentReceived(ContentReceivedEventArgs(chunk), this);
			_stateMachine.Fire(Trigger_Continue);
		}
	}

	/**
	 * Called when the chunk size should attempt to be parsed.
	 * If it is successfully parsed, the state machine moves to the next state.
	 * If partial data was received, the state machine waits for more data.
	 */
	void ChunkSizeEntered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		size_t endOfSize;

		if ((endOfSize = _buffer.find("\r\n")) == std::string::npos) {
			return;
		}
		else {
			_contentLength = String::Convert<size_t>(_buffer.substr(0, endOfSize), std::hex);
			_buffer.erase(0, endOfSize + 2);

			if (_contentLength == 0) {
				_buffer.erase(0, endOfSize + 2);
				_stateMachine.Fire(Trigger_EndOfChunks);
			}
			else {
				_stateMachine.Fire(Trigger_Break);
			}
		}
	}

	/**
	 * Called when a chunk should attempt to be received.
	 * If partial data was received, the state machine waits for more data.
	 */
	void ChunkEntered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_contentLength == 0) {
			if (_buffer.size() >= 2 && _buffer.substr(0, 2) == "\r\n") {
				_buffer.erase(0, 2);
				_stateMachine.Fire(Trigger_Break);
			}
		}
		else if (_buffer.empty() == false) {
			size_t count = _buffer.size();

			if (_contentLength < count) {
				count = _contentLength;
			}

			std::string chunk = _buffer.substr(0, count);
			_buffer.erase(0, count);
			_contentLength -= count;

			_contentReceived(ContentReceivedEventArgs(chunk), this);
			_stateMachine.Fire(Trigger_Continue);
		}
	}

	/**
	 * Called when a request has been completely parsed and has ended for a connection close response.
	 */
	void EndOfResponseContentUntilClosedEntered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		EndEntered();
		_stateMachine.Fire(Trigger_Break);
	}

    /**
	 * Called when a request has been completely parsed and has ended.
	 */
	void EndEntered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_responseEnded(ResponseEndedEventArgs(), this);
	}

public:

	/**
	 * Creates a new http client.
	 */
	HttpClient() : _stateMachine(State_WaitForConnection), _buffer(), _contentLength(0), _responseStarted(), _headerReceived(), _contentReceived(), _responseEnded() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		ClientConnected() += delegate(&HttpClient::OnClientConnected, this);
		DataReceived() += delegate(&HttpClient::OnDataReceived, this);
		ClientDisconnected() += delegate(&HttpClient::OnClientDisconnected, this);

		_stateMachine.Configure(State_WaitForConnection)
			.OnEntry(delegate(&HttpClient::OnWaitForConnectionEntered, this))
			.Permit(Trigger_Continue, State_WaitForConnection)
			.Permit(Trigger_Break, State_WaitForConnection)
			.Permit(Trigger_Connected, State_Connected);

		_stateMachine.Configure(State_Connected)
			.Permit(Trigger_Disconnect, State_WaitForConnection)
			.Permit(Trigger_RequestBegin, State_RequestActionLine);

		_stateMachine.Configure(State_RequestActionLine)
			.Permit(Trigger_Disconnect, State_WaitForConnection)
			.Permit(Trigger_RequestHeader, State_RequestHeaderLine)
			.Permit(Trigger_RequestChunk, State_RequestLastHeader);

		_stateMachine.Configure(State_RequestHeaderLine)
			.Permit(Trigger_Disconnect, State_WaitForConnection)
			.Permit(Trigger_RequestHeader, State_RequestHeaderLine)
			.Permit(Trigger_RequestChunk, State_RequestLastHeader);

		_stateMachine.Configure(State_RequestLastHeader)
			.OnEntry(delegate(&HttpClient::OnLastHeaderEntered, this))
			.Permit(Trigger_Disconnect, State_WaitForConnection)
			.Permit(Trigger_Break, State_RequestChunk);

		_stateMachine.Configure(State_RequestChunk)
			.Permit(Trigger_Disconnect, State_WaitForConnection)
			.Permit(Trigger_RequestChunk, State_RequestChunk)
			.Permit(Trigger_RequestEnd, State_ResponseActionLine);

		_stateMachine.Configure(State_ResponseActionLine)
			.OnEntry(delegate(&HttpClient::ActionLineEntered, this))
			.Permit(Trigger_Disconnect, State_WaitForConnection)
			.Permit(Trigger_Continue, State_ResponseActionLine)
			.Permit(Trigger_Break, State_ResponseHeaderLine);

		_stateMachine.Configure(State_ResponseHeaderLine)
			.OnEntry(delegate(&HttpClient::HeaderLineEntered, this))
			.Permit(Trigger_Disconnect, State_WaitForConnection)
			.Permit(Trigger_Continue, State_ResponseHeaderLine)
			.Permit(Trigger_TransferEncodingChunked, State_ResponseHeaderLineAndTransferEncodingChunked)
			.Permit(Trigger_ContentLength, State_ResponseHeaderLineAndContentLength)
			.Permit(Trigger_ConnectionClose, State_ResponseHeaderLineConnectionClose)
			.Permit(Trigger_Break, State_ResponseContent);

		_stateMachine.Configure(State_ResponseHeaderLineAndTransferEncodingChunked)
			.OnEntry(delegate(&HttpClient::HeaderLineEntered, this))
			.Permit(Trigger_Disconnect, State_WaitForConnection)
			.Permit(Trigger_Continue, State_ResponseHeaderLineAndTransferEncodingChunked)
			.Permit(Trigger_ConnectionClose, State_ResponseHeaderLineAndTransferEncodingChunkedAndConnectionClose)
			.Permit(Trigger_Break, State_ResponseChunkSize);

		_stateMachine.Configure(State_ResponseHeaderLineAndContentLength)
			.OnEntry(delegate(&HttpClient::HeaderLineEntered, this))
			.Permit(Trigger_Disconnect, State_WaitForConnection)
			.Permit(Trigger_ConnectionClose, State_ResponseHeaderLineAndContentLengthAndConnectionClose)
			.Permit(Trigger_Continue, State_ResponseHeaderLineAndContentLength)
			.Permit(Trigger_Break, State_ResponseContent);

		_stateMachine.Configure(State_ResponseHeaderLineConnectionClose)
			.OnEntry(delegate(&HttpClient::HeaderLineEntered, this))
			.Permit(Trigger_Disconnect, State_WaitForConnection)
			.Permit(Trigger_ContentLength, State_ResponseHeaderLineAndContentLengthAndConnectionClose)
			.Permit(Trigger_TransferEncodingChunked, State_ResponseHeaderLineAndTransferEncodingChunkedAndConnectionClose)
			.Permit(Trigger_Continue, State_ResponseHeaderLineConnectionClose)
			.Permit(Trigger_Break, State_ResponseContentUntilClosed);

		_stateMachine.Configure(State_ResponseHeaderLineAndContentLengthAndConnectionClose)
			.OnEntry(delegate(&HttpClient::HeaderLineEntered, this))
			.Permit(Trigger_Disconnect, State_WaitForConnection)
			.Permit(Trigger_Continue, State_ResponseHeaderLineAndContentLengthAndConnectionClose)
			.Permit(Trigger_Break, State_ResponseContentUntilClosed);

		_stateMachine.Configure(State_ResponseHeaderLineAndTransferEncodingChunkedAndConnectionClose)
			.OnEntry(delegate(&HttpClient::HeaderLineEntered, this))
			.Permit(Trigger_Disconnect, State_WaitForConnection)
			.Permit(Trigger_Continue, State_ResponseHeaderLineAndTransferEncodingChunkedAndConnectionClose)
			.Permit(Trigger_Break, State_ResponseChunkSizeAndConnectionClose);

		_stateMachine.Configure(State_ResponseContentUntilClosed)
			.OnEntry(delegate(&HttpClient::ContentUntilClosedEntered, this))
			.Permit(Trigger_Disconnect, State_EndOfResponseContentUntilClosed)
			.Permit(Trigger_Continue, State_ResponseContentUntilClosed)
			.Permit(Trigger_Break, State_EndOfResponseContentUntilClosed);

		_stateMachine.Configure(State_ResponseContent)
			.OnEntry(delegate(&HttpClient::ContentEntered, this))
			.Permit(Trigger_Disconnect, State_WaitForConnection)
			.Permit(Trigger_Continue, State_ResponseContent)
			.Permit(Trigger_Break, State_EndOfResponse);

		_stateMachine.Configure(State_ResponseChunkSize)
			.OnEntry(delegate(&HttpClient::ChunkSizeEntered, this))
			.Permit(Trigger_Disconnect, State_WaitForConnection)
			.Permit(Trigger_Continue, State_ResponseChunkSize)
			.Permit(Trigger_EndOfChunks, State_EndOfResponse)
			.Permit(Trigger_Break, State_ResponseChunk);

		_stateMachine.Configure(State_ResponseChunkSizeAndConnectionClose)
			.OnEntry(delegate(&HttpClient::ChunkSizeEntered, this))
			.Permit(Trigger_Disconnect, State_WaitForConnection)
			.Permit(Trigger_Continue, State_ResponseChunkSizeAndConnectionClose)
			.Permit(Trigger_EndOfChunks, State_EndOfResponseContentUntilClosed)
			.Permit(Trigger_Break, State_ResponseChunkAndConnectionClose);

		_stateMachine.Configure(State_ResponseChunk)
			.OnEntry(delegate(&HttpClient::ChunkEntered, this))
			.Permit(Trigger_Disconnect, State_WaitForConnection)
			.Permit(Trigger_Continue, State_ResponseChunk)
			.Permit(Trigger_Break, State_ResponseChunkSize);

		_stateMachine.Configure(State_ResponseChunkAndConnectionClose)
			.OnEntry(delegate(&HttpClient::ChunkEntered, this))
			.Permit(Trigger_Disconnect, State_WaitForConnection)
			.Permit(Trigger_Continue, State_ResponseChunkAndConnectionClose)
			.Permit(Trigger_Break, State_ResponseChunkSizeAndConnectionClose);

		_stateMachine.Configure(State_EndOfResponseContentUntilClosed)
			.OnEntry(delegate(&HttpClient::EndOfResponseContentUntilClosedEntered, this))
			.Permit(Trigger_Disconnect, State_WaitForConnection)
			.Permit(Trigger_Continue, State_EndOfResponseContentUntilClosed)
			.Permit(Trigger_Break, State_WaitForDisconnect);

		_stateMachine.Configure(State_WaitForDisconnect)
			.Permit(Trigger_Disconnect, State_WaitForConnection);

		_stateMachine.Configure(State_EndOfResponse)
			.OnEntry(delegate(&HttpClient::EndEntered, this))
			.Permit(Trigger_Disconnect, State_WaitForConnection)
			.Permit(Trigger_RequestBegin, State_RequestActionLine);
	}

	/**
	 * The event used to notify when the start of a client response has been received.
	 *
	 * @return The event.
	 */
	ResponseStartedEvent& ResponseStarted() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _responseStarted;
	}

	/**
	 * The event used to notify when a response header has been received.
	 *
	 * @return The event.
	 */
	HeaderReceivedEvent& HeaderReceived() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _headerReceived;
	}

	/**
	 * The event used to notify when partial content has been received.
	 *
	 * @return The event.
	 */
	ContentReceivedEvent& ContentReceived() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _contentReceived;
	}

	/**
	 * The event used to notify when the end of a response has been received.
	 *
	 * @return The event.
	 */
	ResponseEndedEvent& ResponseEnded() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _responseEnded;
	}

	/**
	 * Begins an http request.
	 *
	 * @param method The http method.
	 * @param path The path.
	 * @param protocol The http protocol.
	 * @return A reference to this http client.
	 */
	HttpClient& Begin(const std::string& method, const std::string& path, const std::string& protocol) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_stateMachine.Fire(Trigger_RequestBegin);
		TcpClient::Send(method + " " + path + " " + protocol + "\r\n");

		return *this;
	}

	/**
	 * Sends a request header.
	 *
	 * @param key The key of the header.
	 * @param value The value of the header.
	 * @return A reference to this http client.
	 */
	HttpClient& SendHeader(const std::string& key, const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_stateMachine.Fire(Trigger_RequestHeader);
		TcpClient::Send(key + ": " + value + "\r\n");

		return *this;
	}

	/**
	 * Sends partial request content.
	 *
	 * @param data The content.
	 * @return A reference to this http client.
	 */
	HttpClient& Send(const std::string& data) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_stateMachine.Fire(Trigger_RequestChunk);

		if (data.empty() == false) {
			TcpClient::Send(String::Format("%x\r\n", data.size()) + data + "\r\n");
		}

		return *this;
	}

	/**
	 * Ends the http request.
	 *
	 * @return A reference to this http client.
	 */
	HttpClient& End() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_stateMachine.Fire(Trigger_RequestEnd);
		TcpClient::Send("0\r\n\r\n");

		return *this;
	}

	/**
	 * Deletes this http client.
	 */
	virtual ~HttpClient() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}
};

}

#endif /* HTTPCLIENT_HPP_ */
