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

#ifndef HTTPSERVER_HPP_
#define HTTPSERVER_HPP_

#include "../StackTrace.hpp"
#include "../state/StateMachine.hpp"
#include "../net/TcpServer.hpp"

namespace nitrus {

/**
 * A class that provides functionality for accepting clients over the hypertext transfer protocol.
 * The socket communication provided by this class is insecure; therefore, any sensitive data should be handled by an HttpsServer instead.
 */
class HttpServer : public TcpServer {
public:

	/**
	 * A class that provides a state machine for sending and receiving data for a single client.
	 * This class manages the events and synchronization of messages.
	 */
	class HttpClient {
	public:

		/**
		 * A class that encapsulates the start of an HTTP request.
		 */
		class RequestStartedEventArgs : public EventArgs {
		private:
			std::string _method;
			std::string _path;
			std::string _protocol;

		public:

			/**
			 * Creates a new event argument.
			 *
			 * @param method The request method.
			 * @param path The request path.
			 * @param protocol The protocol.
			 */
			RequestStartedEventArgs(const std::string& method, const std::string& path, const std::string& protocol) : _method(method), _path(path), _protocol(protocol) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}

			/**
			 * Returns the method for the request.
			 *
			 * @return The method.
			 */
			const std::string& Method() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _method;
			}

			/**
			 * Returns the path for the request.
			 *
			 * @return The path.
			 */
			const std::string& Path() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _path;
			}

			/**
			 * Returns the protocol for the request.
			 *
			 * @return The protocol.
			 */
			const std::string& Protocol() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _protocol;
			}

			/**
			 * Deletes this event argument.
			 */
			virtual ~RequestStartedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}
		};

		typedef EventHandler<const RequestStartedEventArgs&> RequestStartedEventHandler;
		typedef Event<const RequestStartedEventArgs&> RequestStartedEvent;

		/**
		 * A class that encapsulates a header for an HTTP request.
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
		 * A class that encapsulates content for an HTTP request.
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
			 * The partial content of the request.
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
		 * A class that encapsulates the end of an HTTP request.
		 */
		class RequestEndedEventArgs : public EventArgs {
		public:

			/**
			 * Creates a new event argument.
			 */
			RequestEndedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}

			/**
			 * Deletes this event argument.
			 */
			virtual ~RequestEndedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}
		};

		typedef EventHandler<const RequestEndedEventArgs&> RequestEndedEventHandler;
		typedef Event<const RequestEndedEventArgs&> RequestEndedEvent;

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

	private:

		enum State {
			State_RequestActionLine,
			State_RequestHeaderLine,
			State_RequestHeaderLineAndConnectionClose,
			State_RequestHeaderLineAndContentLength,
			State_RequestHeaderLineAndTransferEncodingChunked,
			State_RequestHeaderLineAndContentLengthAndConnectionClose,
			State_RequestHeaderLineAndTransferEncodingChunkedAndConnectionClose,
			State_RequestContent,
			State_RequestChunkSize,
			State_RequestChunk,
			State_EndOfRequest,
			State_ResponseActionLine,
			State_ResponseHeaderLine,
			State_ResponseLastHeader,
			State_ResponseChunk,
			State_RequestContentAndConnectionClose,
			State_RequestChunkSizeAndConnectionClose,
			State_RequestChunkAndConnectionClose,
			State_EndOfRequestAndConnectionClose,
			State_ResponseActionLineAndConnectionClose,
			State_ResponseHeaderLineAndConnectionClose,
			State_ResponseLastHeaderAndConnectionClose,
			State_ResponseChunkAndConnectionClose,
			State_ConnectionClose
		};

		enum Trigger {
			Trigger_Continue,
			Trigger_Break,
			Trigger_TransferEncodingChunked,
			Trigger_ContentLength,
			Trigger_ConnectionClose,
			Trigger_EndOfChunks,

			Trigger_ResponseBegin,
			Trigger_ResponseHeader,
			Trigger_ResponseChunk,
			Trigger_ResponseEnd
		};

	private:
		StateMachine<State, Trigger> _stateMachine;
		TcpClient* _client;
		Socket::Endpoint _endpoint;
		std::string _buffer;
		RequestStartedEvent _requestStarted;
		HeaderReceivedEvent _headerReceived;
		ContentReceivedEvent _contentReceived;
		RequestEndedEvent _requestEnded;
		size_t _contentLength;
		ClientDisconnectedEvent _clientDisconnected;

	private:
		template <typename Signature> friend class Delegate;

		/**
		 * Called when the tcp client associated with this handler is disconnected.
		 * This will free this handler.
		 *
		 * @param args The event arguments.
		 * @param sender The sender of the event.
		 */
		void OnClientDisconnected(const TcpClient::ClientDisconnectedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_clientDisconnected(ClientDisconnectedEventArgs(), this);
			delete this;
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
		 * Called when the action line should attempt to be parsed.
		 * If the action line is successfully parsed, the state machine moves to the next state.
		 * If partial data was received, the state machine waits for more data.
		 */
		void ActionLineEntered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			size_t endOfMethod, endOfPath, endOfProtocol;

			if ((endOfMethod = _buffer.find(" ")) == std::string::npos) {
				return;
			}
			else if ((endOfPath = _buffer.find(" ", endOfMethod + 1)) == std::string::npos) {
				return;
			}
			else if ((endOfProtocol = _buffer.find("\r\n",  endOfPath + 1)) == std::string::npos) {
				return;
			}

			std::string method = _buffer.substr(0, endOfMethod);
			std::string path = _buffer.substr(endOfMethod + 1, endOfPath - endOfMethod - 1);
			std::string protocol = _buffer.substr(endOfPath + 1, endOfProtocol - endOfPath - 1);
			_buffer.erase(0, endOfProtocol + 2);

			_contentLength = 0;
			_requestStarted(RequestStartedEventArgs(method, path, protocol), this);
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
		 * Called when a request has been completely parsed and has ended.
		 */
		void EndEntered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_requestEnded(RequestEndedEventArgs(), this);
		}

		/**
		 * Called when the last header of a response has been sent.
		 */
		void LastHeaderEntered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_client->Send("Transfer-Encoding: chunked\r\n\r\n");
			_stateMachine.Fire(Trigger_Break);
		}

		/**
		 * Called when the last header of a response has been sent for a request wanting a closed connection after the response.
		 */
		void LastHeaderAndConnectionCloseEntered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_client->Send("Connection: close\r\n\r\n");
			_stateMachine.Fire(Trigger_Break);
		}

		/**
		 * Called when the connection needs to be closed.
		 * Disconnects the client cleanly.
		 */
		void ConnectionCloseEntered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_client->Disconnect();
		}

	public:

		/**
		 * Creates a new http client for the specified tcp client.
		 *
		 * @param client The client that was accepted by the http server.
		 * @param endpoint The endpoint of the client.
		 */
		HttpClient(TcpClient* client, const Socket::Endpoint& endpoint) : _stateMachine(State_RequestActionLine), _client(client), _endpoint(endpoint), _buffer(), _requestStarted(), _headerReceived(), _contentReceived(), _requestEnded(), _contentLength(0), _clientDisconnected() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			client->DataReceived() += delegate(&HttpClient::OnDataReceived, this);
			client->ClientDisconnected() += delegate(&HttpClient::OnClientDisconnected, this);

			_stateMachine.Configure(State_RequestActionLine)
				.OnEntry(delegate(&HttpClient::ActionLineEntered, this))
				.Permit(Trigger_Continue, State_RequestActionLine)
				.Permit(Trigger_Break, State_RequestHeaderLine);

			_stateMachine.Configure(State_RequestHeaderLine)
				.OnEntry(delegate(&HttpClient::HeaderLineEntered, this))
				.Permit(Trigger_Continue, State_RequestHeaderLine)
				.Permit(Trigger_TransferEncodingChunked, State_RequestHeaderLineAndTransferEncodingChunked)
				.Permit(Trigger_ContentLength, State_RequestHeaderLineAndContentLength)
				.Permit(Trigger_ConnectionClose, State_RequestHeaderLineAndConnectionClose)
				.Permit(Trigger_Break, State_RequestContent);

			_stateMachine.Configure(State_RequestHeaderLineAndConnectionClose)
				.OnEntry(delegate(&HttpClient::HeaderLineEntered, this))
				.Permit(Trigger_Continue, State_RequestHeaderLineAndConnectionClose)
				.Permit(Trigger_TransferEncodingChunked, State_RequestHeaderLineAndTransferEncodingChunkedAndConnectionClose)
				.Permit(Trigger_ContentLength, State_RequestHeaderLineAndContentLengthAndConnectionClose)
				.Permit(Trigger_Break, State_RequestContentAndConnectionClose);

			_stateMachine.Configure(State_RequestHeaderLineAndContentLengthAndConnectionClose)
				.OnEntry(delegate(&HttpClient::HeaderLineEntered, this))
				.Permit(Trigger_Continue, State_RequestHeaderLineAndContentLengthAndConnectionClose)
				.Permit(Trigger_Break, State_RequestContentAndConnectionClose);

			_stateMachine.Configure(State_RequestHeaderLineAndTransferEncodingChunkedAndConnectionClose)
				.OnEntry(delegate(&HttpClient::HeaderLineEntered, this))
				.Permit(Trigger_Continue, State_RequestHeaderLineAndTransferEncodingChunkedAndConnectionClose)
				.Permit(Trigger_Break, State_RequestChunkSizeAndConnectionClose);

			_stateMachine.Configure(State_RequestHeaderLineAndTransferEncodingChunked)
				.OnEntry(delegate(&HttpClient::HeaderLineEntered, this))
				.Permit(Trigger_Continue, State_RequestHeaderLineAndTransferEncodingChunked)
				.Permit(Trigger_ConnectionClose, State_RequestHeaderLineAndTransferEncodingChunkedAndConnectionClose)
				.Permit(Trigger_Break, State_RequestChunkSize);

			_stateMachine.Configure(State_RequestHeaderLineAndContentLength)
				.OnEntry(delegate(&HttpClient::HeaderLineEntered, this))
				.Permit(Trigger_Continue, State_RequestHeaderLineAndContentLength)
				.Permit(Trigger_ConnectionClose, State_RequestHeaderLineAndContentLengthAndConnectionClose)
				.Permit(Trigger_Break, State_RequestContent);

			_stateMachine.Configure(State_RequestContent)
				.OnEntry(delegate(&HttpClient::ContentEntered, this))
				.Permit(Trigger_Continue, State_RequestContent)
				.Permit(Trigger_Break, State_EndOfRequest);

			_stateMachine.Configure(State_RequestContentAndConnectionClose)
				.OnEntry(delegate(&HttpClient::ContentEntered, this))
				.Permit(Trigger_Continue, State_RequestContentAndConnectionClose)
				.Permit(Trigger_Break, State_EndOfRequestAndConnectionClose);

			_stateMachine.Configure(State_RequestChunkSize)
				.OnEntry(delegate(&HttpClient::ChunkSizeEntered, this))
				.Permit(Trigger_Continue, State_RequestChunkSize)
				.Permit(Trigger_EndOfChunks, State_EndOfRequest)
				.Permit(Trigger_Break, State_RequestChunk);

			_stateMachine.Configure(State_RequestChunkSizeAndConnectionClose)
				.OnEntry(delegate(&HttpClient::ChunkSizeEntered, this))
				.Permit(Trigger_Continue, State_RequestChunkSizeAndConnectionClose)
				.Permit(Trigger_EndOfChunks, State_EndOfRequestAndConnectionClose)
				.Permit(Trigger_Break, State_RequestChunkAndConnectionClose);

			_stateMachine.Configure(State_RequestChunk)
				.OnEntry(delegate(&HttpClient::ChunkEntered, this))
				.Permit(Trigger_Continue, State_RequestChunk)
				.Permit(Trigger_Break, State_RequestChunkSize);

			_stateMachine.Configure(State_RequestChunkAndConnectionClose)
				.OnEntry(delegate(&HttpClient::ChunkEntered, this))
				.Permit(Trigger_Continue, State_RequestChunkAndConnectionClose)
				.Permit(Trigger_Break, State_RequestChunkSizeAndConnectionClose);

			_stateMachine.Configure(State_EndOfRequest)
				.OnEntry(delegate(&HttpClient::EndEntered, this))
				.Permit(Trigger_ResponseBegin, State_ResponseActionLine);

			_stateMachine.Configure(State_EndOfRequestAndConnectionClose)
				.OnEntry(delegate(&HttpClient::EndEntered, this))
				.Permit(Trigger_ResponseBegin, State_ResponseActionLineAndConnectionClose);

			_stateMachine.Configure(State_ResponseActionLine)
					.Permit(Trigger_ResponseHeader, State_ResponseHeaderLine);

			_stateMachine.Configure(State_ResponseActionLineAndConnectionClose)
				.Permit(Trigger_ResponseHeader, State_ResponseHeaderLineAndConnectionClose);

			_stateMachine.Configure(State_ResponseHeaderLine)
				.Permit(Trigger_ResponseHeader, State_ResponseHeaderLine)
				.Permit(Trigger_ResponseChunk, State_ResponseLastHeader);

			_stateMachine.Configure(State_ResponseHeaderLineAndConnectionClose)
				.Permit(Trigger_ResponseHeader, State_ResponseHeaderLineAndConnectionClose)
				.Permit(Trigger_ResponseChunk, State_ResponseLastHeaderAndConnectionClose);

			_stateMachine.Configure(State_ResponseLastHeader)
				.OnEntry(delegate(&HttpClient::LastHeaderEntered, this))
				.Permit(Trigger_Break, State_ResponseChunk);

			_stateMachine.Configure(State_ResponseLastHeaderAndConnectionClose)
				.OnEntry(delegate(&HttpClient::LastHeaderAndConnectionCloseEntered, this))
				.Permit(Trigger_Break, State_ResponseChunkAndConnectionClose);

			_stateMachine.Configure(State_ResponseChunk)
				.Permit(Trigger_ResponseChunk, State_ResponseChunk)
				.Permit(Trigger_ResponseEnd, State_RequestActionLine);

			_stateMachine.Configure(State_ResponseChunkAndConnectionClose)
				.Permit(Trigger_ResponseChunk, State_ResponseChunkAndConnectionClose)
				.Permit(Trigger_ResponseEnd, State_ConnectionClose);

			_stateMachine.Configure(State_ConnectionClose)
				.OnEntry(delegate(&HttpClient::ConnectionCloseEntered, this));
		}

		/**
		 * Creates a new http client by copying another http client.
		 *
		 * @param that The client to clone.
		 */
		HttpClient(const HttpClient& that) : _stateMachine(that._stateMachine), _client(that._client), _endpoint(that._endpoint), _buffer(that._buffer), _requestStarted(that._requestStarted), _headerReceived(that._headerReceived), _contentReceived(that._contentReceived), _requestEnded(that._requestEnded), _contentLength(that._contentLength), _clientDisconnected(that._clientDisconnected) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Copies the specified http client into this http client.
		 *
		 * @param that The client to clone.
		 * @return A reference to this http client.
		 */
		HttpClient& operator = (const HttpClient& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_stateMachine = that._stateMachine;
			_client = that._client;
			_endpoint = that._endpoint;
			_buffer = that._buffer;
			_requestStarted = that._requestStarted;
			_headerReceived = that._headerReceived;
			_contentReceived = that._contentReceived;
			_requestEnded = that._requestEnded;
			_contentLength = that._contentLength;
			_clientDisconnected = that._clientDisconnected;

			return *this;
		}

		/**
		 * The event used to notify when the start of a client request has been received.
		 *
		 * @return The event.
		 */
		RequestStartedEvent& RequestStarted() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _requestStarted;
		}

		/**
		 * The event used to notify when a request header has been received.
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
		 * The event used to notify when the end of a request has been received.
		 *
		 * @return The event.
		 */
		RequestEndedEvent& RequestEnded() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _requestEnded;
		}

		/**
		 * Begins a response to a request.
		 *
		 * @param protocol The protocol.
		 * @param code The response code.
		 * @param description The description of the response code.
		 * @return A reference to this http client.
		 */
		HttpClient& Begin(const char* protocol, int code, const char* description) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_stateMachine.Fire(Trigger_ResponseBegin);
			_client->Send(String::Format("%s %d %s\r\n", protocol, code, description));

			return *this;
		}

		/**
		 * Sends a response header.
		 *
		 * @param key The key of the header.
		 * @param value The value of the header.
		 * @return A reference to this http client.
		 */
		HttpClient& SendHeader(const std::string& key, const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_stateMachine.Fire(Trigger_ResponseHeader);
			_client->Send(key + ": " + value + "\r\n");

			return *this;
		}

		/**
		 * Sends partial response content.
		 *
		 * @param data The content.
		 * @return A reference to this http client.
		 */
		HttpClient& Send(const std::string& data) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_stateMachine.Fire(Trigger_ResponseChunk);

			if (data.empty() == false) {
				if (_stateMachine.State() == State_ResponseChunk) {
					_client->Send(String::Format("%x\r\n", data.size()) + data + "\r\n");
				}
				else {
					_client->Send(data);
				}
			}

			return *this;
		}

		/**
		 * Ends a response to a request.
		 *
		 * @return A reference to this http client.
		 */
		HttpClient& End() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			if (_stateMachine.State() == State_ResponseChunk) {
				_client->Send("0\r\n\r\n");
			}

			_stateMachine.Fire(Trigger_ResponseEnd);
			return *this;
		}

		/**
		 * The event used to notify listeners when a client is disconnected.
		 *
		 * @return The event.
		 */
		ClientDisconnectedEvent& ClientDisconnected() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _clientDisconnected;
		}

		/**
		 * Deletes this client handler.
		 */
		virtual ~HttpClient() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_client->DataReceived() -= delegate(&HttpClient::OnDataReceived, this);
			_client->ClientDisconnected() -= delegate(&HttpClient::OnClientDisconnected, this);
		}
	};

	/**
	 * A class that encapsulates a client that has been accepted by the http server.
	 */
	class ClientAcceptedEventArgs : public EventArgs {
	private:
		HttpClient* _client;

	public:

		/**
		 * Creates a new event argument.
		 *
		 * @param client The client that was accepted.
		 */
		ClientAcceptedEventArgs(HttpClient* client) : _client(client) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Creates a new event argument from the specified argument.
		 *
		 * @param that The event argument to clone.
		 */
		ClientAcceptedEventArgs(const ClientAcceptedEventArgs& that) : _client(that._client) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		/**
		 * Copies the specified event argument to this event argument.
		 *
		 * @param that The event argument to clone.
		 * @return A reference to this event argument.
		 */
		ClientAcceptedEventArgs& operator = (const ClientAcceptedEventArgs& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_client = that._client;
			return *this;
		}

		/**
		 * The client that has been accepted.
		 *
		 * @return The client.
		 */
		HttpClient* Client() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _client;
		}

		/**
		 * Deletes this event argument.
		 */
		virtual ~ClientAcceptedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	typedef EventHandler<const ClientAcceptedEventArgs&> ClientAcceptedEventHandler;
	typedef Event<const ClientAcceptedEventArgs&> ClientAcceptedEvent;

private:
	ClientAcceptedEvent _clientAccepted;

private:
	template <typename Signature> friend class Delegate;

	/**
	 * Called when a client has been accepted by the http server.
	 * This will create a new handler for the client and manage its state.
	 *
	 * @param args The event arguments.
	 * @param sender The sender of the event.
	 */
	void OnClientAccepted(const TcpServer::ClientAcceptedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		HttpClient* client = new HttpClient(args.Client(), args.Endpoint());
		_clientAccepted(ClientAcceptedEventArgs(client), this);
	}

public:

	/**
	 * Creates a new http server that listens for http client connections.
	 */
	HttpServer() : _clientAccepted() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		TcpServer::ClientAccepted() += delegate(&HttpServer::OnClientAccepted, this);
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
	 * Deletes this http server and stops listening for http client connections.
	 */
	virtual ~HttpServer() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		TcpServer::ClientAccepted() -= delegate(&HttpServer::OnClientAccepted, this);
	}
};

}

#endif /* HTTPSERVER_HPP_ */
