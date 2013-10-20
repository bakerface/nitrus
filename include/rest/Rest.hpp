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

#ifndef REST_HPP_
#define REST_HPP_

#include "../StackTrace.hpp"
#include "../Log.hpp"
#include "../Event.hpp"
#include "../String.hpp"
#include "../state/StateMachine.hpp"
#include "../http/HttpServer.hpp"
#include "../fs/File.hpp"
#include "../fs/Directory.hpp"

#include <map>

namespace nitrus {

/**
 * A class that provides functionality for creating a REST web server.
 */
class Rest {
public:

	/**
	 * A class that provides functionality for routing web requests to handler functions.
	 */
	class Router : public HttpServer {
	public:
		typedef std::multimap<std::string, std::string> HeaderCollection;
		typedef std::map<std::string, std::string> MatchCollection;

		/**
		 * A class that encapsulates a web request received from an http client.
		 */
		class RequestEventArgs : public EventArgs {
		private:
			HttpServer::HttpClient* _client;
			std::string _method;
			std::string _path;
			HeaderCollection _headers;
			std::string _content;
			MatchCollection _matches;

		public:

			/**
			 * Creates a new event argument for a web request.
			 *
			 * @param client The client that sent the request.
			 * @param method The http method.
			 * @param path The path requested.
			 * @param headers The request headers.
			 * @param content The request content.
			 * @param matches The matched routing keys and values.
			 */
			RequestEventArgs(HttpServer::HttpClient* client, const std::string& method, const std::string& path, const HeaderCollection& headers, const std::string& content, const MatchCollection& matches) : _client(client), _method(method), _path(path), _headers(headers), _content(content), _matches(matches) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}

			/**
			 * Creates a new event argument for a web request from the specified event argument.
			 *
			 * @param that The request to clone.
			 */
			RequestEventArgs(const RequestEventArgs& that) : _client(that._client), _method(that._method), _path(that._path), _headers(that._headers), _content(that._content), _matches(that._matches) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}

			/**
			 * Copies the specified event argument into this event argument.
			 *
			 * @param that The event argument to clone.
			 * @return A reference to this event argument.
			 */
			RequestEventArgs& operator = (const RequestEventArgs& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				_client = that._client;
				_method = that._method;
				_path = that._path;
				_headers = that._headers;
				_content = that._content;
				_matches = that._matches;

				return *this;
			}

			/**
			 * The client that sent the request.
			 *
			 * @return The client.
			 */
			HttpServer::HttpClient* Client() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _client;
			}

			/**
			 * The http method.
			 *
			 * @return The method.
			 */
			const std::string& Method() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _method;
			}

			/**
			 * The http path requested.
			 *
			 * @return The path.
			 */
			const std::string& Path() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _path;
			}

			/**
			 * The http request headers.
			 *
			 * @return The headers.
			 */
			const HeaderCollection& Headers() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _headers;
			}

			/**
			 * The http request content.
			 *
			 * @return The content.
			 */
			const std::string& Content() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _content;
			}

			/**
			 * The matched routing keys and values.
			 *
			 * @return The matches.
			 */
			const MatchCollection& Matches() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _matches;
			}

			/**
			 * The matched routing keys and values.
			 *
			 * @return The matches.
			 */
			MatchCollection& Matches() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _matches;
			}

			/**
			 * Gets the matched value for the specified routing key.
			 *
			 * @param key The routing key.
			 * @param defaultValue The default value to return if not found.
			 * @return The matched value if the key exists, the default value otherwise.
			 */
			std::string Match(const std::string& key, const std::string& defaultValue = "") const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				MatchCollection::const_iterator i = _matches.find(key);
				return i == _matches.end() ? defaultValue : i->second;
			}

			/**
			 * Gets the matched value for the specified routing key.
			 *
			 * @param key The routing key.
			 * @param defaultValue The default value to return if not found.
			 * @return The matched value if the key exists, the default value otherwise.
			 */
			template <typename T> T Match(const std::string& key, const T& defaultValue = T()) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				MatchCollection::const_iterator i = _matches.find(key);
				return i == _matches.end() ? defaultValue : String::Convert<T>(i->second);
			}

			/**
			 * Deletes this event argument.
			 */
			virtual ~RequestEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}
		};

		typedef EventHandler<const RequestEventArgs&> RequestEventHandler;
		typedef Event<const RequestEventArgs&> RequestEvent;

		/**
		 * A class that encapsulates a routing configuration.
		 */
		class Configuration {
		private:
			typedef std::map<std::string, RequestEventHandler> Handlers;
			Handlers _handlers;

		public:

			/**
			 * Creates a new routing configuration.
			 */
			Configuration() : _handlers() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}

			/**
			 * Invokes the handler for the specified request event.
			 *
			 * @param args The request event arguments.
			 * @param sender The sender of the arguments.
			 * @return True if the request could be handled, false otherwise.
			 */
			bool operator ()(const RequestEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				Handlers::iterator i = _handlers.find(String::ToUpperCase(args.Method()));

				if (i == _handlers.end()) {
					return false;
				}

				try {
					i->second(args, sender);
				}
				catch (...) {
					args.Client()->Begin("HTTP/1.1", 400, "Bad Request")
						.SendHeader("Server", "nitrus")
						.SendHeader("Content-Type", "text/plain")
						.Send(StackTrace::ToExceptionString())
						.End();
				}

				return true;
			}

			/**
			 * Binds an event handler to an http method for this configuration.
			 *
			 * @param method The method to bind to.
			 * @param handler The event handler to bind.
			 * @return A reference to this configuration.
			 */
			Configuration& Bind(const std::string& method, const RequestEventHandler& handler) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				_handlers[String::ToUpperCase(method)] = handler;
				return *this;
			}

			/**
			 * Binds an event handler to the http GET method for this configuration.
			 *
			 * @param handler The event handler to bind.
			 * @return A reference to this configuration.
			 */
			Configuration& Get(const RequestEventHandler& handler) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return Bind("GET", handler);
			}

			/**
			 * Binds an event handler to the http PUT method for this configuration.
			 *
			 * @param handler The event handler to bind.
			 * @return A reference to this configuration.
			 */
			Configuration& Put(const RequestEventHandler& handler) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return Bind("PUT", handler);
			}

			/**
			 * Binds an event handler to the http POST method for this configuration.
			 *
			 * @param handler The event handler to bind.
			 * @return A reference to this configuration.
			 */
			Configuration& Post(const RequestEventHandler& handler) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return Bind("POST", handler);
			}

			/**
			 * Binds an event handler to the http DELETE method for this configuration.
			 *
			 * @param handler The event handler to bind.
			 * @return A reference to this configuration.
			 */
			Configuration& Delete(const RequestEventHandler& handler) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return Bind("DELETE", handler);
			}

			/**
			 * Deletes this routing configuration.
			 */
			virtual ~Configuration() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}
		};

	private:

		/**
		 * A class that encapsulates the routing logic for an http client.
		 */
		class ClientHandler : public EventArgs {
		private:
			RequestEventHandler _requestHandler;
			HttpServer::HttpClient* _client;
			std::string _method;
			std::string _path;
			HeaderCollection _headers;
			std::string _content;

		private:

			/**
			 * Called when a client request has been started.
			 * This will initialize some member variables that will be used after the request has completed.
			 *
			 * @param args The event arguments.
			 * @param sender The sender of the event.
			 */
			void OnClientRequestStarted(const HttpClient::RequestStartedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				_method = args.Method();
				_path = args.Path();
				_headers.clear();
				_content.clear();
			}

			/**
			 * Called when a single header key and value has been received.
			 * This will add the key and value to a collection that will be used after the request has completed.
			 *
			 * @param args The event arguments.
			 * @param sender The sender of the event.
			 */
			void OnHeaderReceived(const HttpClient::HeaderReceivedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				_headers.insert(std::make_pair(args.Key(), args.Value()));
			}

			/**
			 * Called when partial content has been received
			 * This will append to a content buffer that will be used after the request has completed.
			 *
			 * @param args The event arguments.
			 * @param sender The sender of the event.
			 */
			void OnContentReceived(const HttpClient::ContentReceivedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				_content += args.Content();
			}

			/**
			 * Called when a request has ended.
			 * This will call the event handler passing an event argument that is a collection of all of the http variables.
			 *
			 * @param args The event arguments.
			 * @param sender The sender of the event.
			 */
			void OnClientRequestEnded(const HttpClient::RequestEndedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				_requestHandler(RequestEventArgs(_client, _method, _path, _headers, _content, MatchCollection()), this);
			}

			/**
			 * Called when the client that this object is handling has disconnected.
			 * This will delete the reference to this object.
			 *
			 * @param args The event arguments.
			 * @param sender The sender of the event.
			 */
			void OnClientDisconnected(const HttpClient::ClientDisconnectedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				delete this;
			}

		public:

			/**
			 * Creates a new client handler to manage http requests.
			 *
			 * @param requestHandler The event handler to invoke when a request has been received.
			 * @param client The client to manage.
			 */
			ClientHandler(const RequestEventHandler& requestHandler, HttpServer::HttpClient* client) : _requestHandler(requestHandler), _client(client), _method(), _path(), _headers(), _content() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				client->RequestStarted() += delegate(&ClientHandler::OnClientRequestStarted, this);
				client->HeaderReceived() += delegate(&ClientHandler::OnHeaderReceived, this);
				client->ContentReceived() += delegate(&ClientHandler::OnContentReceived, this);
				client->RequestEnded() += delegate(&ClientHandler::OnClientRequestEnded, this);
				client->ClientDisconnected() += delegate(&ClientHandler::OnClientDisconnected, this);
			}

			/**
			 * Creates a new client handler from the specified client handler.
			 *
			 * @param that The client handler to clone.
			 */
			ClientHandler(const ClientHandler& that) : _requestHandler(that._requestHandler), _client(that._client), _method(that._method), _path(that._path), _headers(that._headers), _content(that._content) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}

			/**
			 * Copies the specified client handler into this client handler.
			 *
			 * @param that The client handler to clone.
			 * @return A reference to this client handler.
			 */
			ClientHandler& operator = (const ClientHandler& that) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				_requestHandler = that._requestHandler;
				_client = that._client;
				_method = that._method;
				_path = that._path;
				_headers = that._headers;
				_content = that._content;

				return *this;
			}

			/**
			 * Deletes this client handler.
			 */
			virtual ~ClientHandler() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}
		};

		/**
		 * A class that encapsulates a comparison between a routing expression and a request path.
		 */
		class ExpressionComparer {
		private:

			/**
			 * Extracts the name of the replaceable routing key.
			 *
			 * @param value The expression to parse.
			 * @param key The buffer to store the routing key. If successful, this will be assigned the name of the key.
			 * @return True if parsing was successful, false otherwise.
			 */
			static bool GetReplaceable(const std::string& value, std::string& key) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				if (value.empty() == false && value.at(0) == '{' && *(value.rbegin()) == '}') {
					key = value.substr(1, value.size() - 2);
					return true;
				}

				return false;
			}

			/**
			 * Determines whether an expression and a base path are considered equal.
			 * If the expression contains routing keys, the key value pair is inserted into a match collection.
			 *
			 * @param expression The expression to match against.
			 * @param path The path used to match.
			 * @param matches A collection used to store the routing keys and values.
			 * @return True if the path matches the expression, false otherwise.
			 */
			static bool PathsAreEqual(const std::string& expression, const std::string& path, MatchCollection& matches) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				std::vector<std::string> expressions = String::Split(expression, '/');
				std::vector<std::string> paths = String::Split(path, '/');
				std::string key;

				if (expressions.size() != paths.size()) {
					return false;
				}

				for (size_t i = 0; i != expressions.size(); i++) {
					if (expressions[i] == paths[i]) {
						// we have matched the path
					}
					else if (GetReplaceable(expressions[i], key)) {
						// we have matched the expression
						matches[key] = paths[i];
					}
					else {
						return false;
					}
				}

				return true;
			}

			/**
			 * Determines whether an expression and a query parameter string are considered equal.
			 * If the expression contains routing keys, the key value pair is inserted into a match collection.
			 *
			 * @param expression The expression to match against.
			 * @param parameter The query parameters used to match.
			 * @param matches A collection used to store the routing keys and values.
			 * @return True if the parameters match the expression, false otherwise.
			 */
			static bool ParametersAreEqual(const std::string& expression, const std::string& parameter, MatchCollection& matches) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				std::vector<std::string> expressions = String::Split(expression, '&');
				std::vector<std::string> parameters = String::Split(parameter, '&');

				if (expressions.size() != parameters.size()) {
					return false;
				}

				for (size_t i = 0; i != expressions.size(); i++) {
					std::string key;
					std::vector<std::string> expressionKeyAndValue = String::Split(expressions[i], '=');
					std::vector<std::string> parameterKeyAndValue = String::Split(parameters[i], '=');

					if (expressionKeyAndValue.size() != parameterKeyAndValue.size()) {
						return false;
					}
					else if (expressionKeyAndValue.size() == 1 && expressionKeyAndValue[0] == parameterKeyAndValue[0]) {
						// we have matched the parameter
					}
					else if (expressionKeyAndValue.size() == 2 && expressionKeyAndValue[0] == parameterKeyAndValue[0] && GetReplaceable(expressionKeyAndValue[1], key)) {
						// we have matched the expression
						matches[key] = parameterKeyAndValue[1];
					}
					else if (expressionKeyAndValue.size() == 2 && expressionKeyAndValue[0] == parameterKeyAndValue[0] && expressionKeyAndValue[1] == parameterKeyAndValue[1]) {
						// we have matched the key and value
					}
					else {
						return false;
					}
				}

				return true;
			}

		public:

			/**
			 * Determines whether an expression and a path are considered equal.
			 * If the expression contains routing keys, the key value pair is inserted into a match collection.
			 *
			 * @param expression The expression to match against.
			 * @param path The path used to match.
			 * @param matches A collection used to store the routing keys and values.
			 * @return True if the path matches the expression, false otherwise.
			 */
			static bool AreEqual(const std::string& expression, const std::string& path, MatchCollection& matches) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				std::vector<std::string> expressionAndParameters = String::Split(expression, '?');
				std::vector<std::string> pathAndParameters = String::Split(path, '?');

				if (expressionAndParameters.size() == 1 && pathAndParameters.size() == 1) {
					return PathsAreEqual(expressionAndParameters[0], pathAndParameters[0], matches);
				}
				else if (expressionAndParameters.size() == 2 && pathAndParameters.size() == 2) {
					return PathsAreEqual(expressionAndParameters[0], pathAndParameters[0], matches) && ParametersAreEqual(expressionAndParameters[1], pathAndParameters[1], matches);
				}

				return false;
			}
		};

		/**
		 * A class that provides functionality for responding to a web request with file contents.
		 */
		class FileHandler {
		private:
			RequestEventArgs _args;

		private:

			/**
			 * Called when a chunk of a file has been read.
			 *
			 * @param args The event arguments.
			 * @param sender The sender of the event.
			 */
			void OnFileChunkRead(const File::ChunkReadEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				_args.Client()->Send(args.Data());
			}

			/**
			 * Called after the last chunk of a file has been read.
			 * This will automatically free this file handler.
			 *
			 * @param args The event arguments.
			 * @param sender The sender of the event.
			 */
			void OnFileEnded(const File::EndOfFileEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				_args.Client()->End();
				delete this;
			}

		public:

			/**
			 * Creates a new file handler.
			 * If the requested path is a directory the response will redirect to the "index.html" page in the directory.
			 * If the requested path is a file the response will be the file contents.
			 * If the requested path could not be found the response will be a 404 Not Found response.
			 *
			 * @param args The request event arguments.
			 * @param documentRoot The root directory containing the web documents.
			 */
			FileHandler(const RequestEventArgs& args, const std::string& documentRoot) : _args(args) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				if (Directory::Exists(documentRoot + args.Path())) {
					args.Client()->Begin("HTTP/1.1", 303, "See Other")
						.SendHeader("Server", "nitrus")
						.SendHeader("Location", args.Path() + "/index.html")
						.Send("")
						.End();
				}
				else {
					try {
						File::Read(documentRoot + args.Path(), File::ChunkReadEventHandler(&FileHandler::OnFileChunkRead, this), File::EndOfFileEventHandler(&FileHandler::OnFileEnded, this));
						args.Client()->Begin("HTTP/1.1", 200, "OK")
							.SendHeader("Server", "nitrus");
					}
					catch (const File::FileNotFoundException& e) {
						args.Client()->Begin("HTTP/1.1", 404, "Not Found")
							.SendHeader("Server", "nitrus")
							.SendHeader("Content-Type", "text/plain")
							.Send("")
							.End();
					}
				}
			}

			/**
			 * Deletes this file handler.
			 */
			virtual ~FileHandler() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}
		};


	private:
		typedef std::map<std::string, Configuration> Configurations;
		Configurations _configurations;
		std::string _documentRoot;

	private:

		/**
		 * Called when a request has been received by the client handler.
		 * This will search for a route that matches the request.
		 * The first route found will be invoked (this may or may not be the first configuration defined so be careful to not define overlapping configurations).
		 * If a route was not found, the undefined route handler is invoked.
		 * If no undefined route handler was specified, a 404 Not Found response is returned.
		 *
		 * @param args The event arguments.
		 * @param sender The sender of the event.
		 */
		void OnClientHandlerRequestReceived(const RequestEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			for (Configurations::iterator i = _configurations.begin(); i != _configurations.end(); i++) {
				RequestEventArgs arguments = args;

				if (ExpressionComparer::AreEqual(i->first, arguments.Path(), arguments.Matches()) && i->second(arguments, this)) {
					return;
				}
			}

			new FileHandler(args, _documentRoot);
		}

		/**
		 * Called when a client has been accepted by the http server.
		 * This will create a new client handler that will manage the requests from the client.
		 *
		 * @param args The event arguments.
		 * @param sender The sender of the event.
		 */
		void OnClientAccepted(const HttpServer::ClientAcceptedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			new ClientHandler(RequestEventHandler(&Router::OnClientHandlerRequestReceived, this), args.Client());
		}

	public:

		/**
		 * Creates a new web request router.
		 *
		 * @param documentRoot The root directory for serving web content from disk.
		 * @param undefinedRouteHandler The event handler invoked when a request does not match a pre-configured route.
		 */
		Router(const std::string documentRoot = "") : _configurations(), _documentRoot(documentRoot) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			ClientAccepted() += delegate(&Router::OnClientAccepted, this);
		}

		/**
		 * Configures the event handlers for the requests matching the speicified routing expression.
		 * Auto-replaceable routing keys are defined by using curly braces around key names.
		 * For example the expression /users/{userId} would match /users/bob and /users/billy.
		 * Similarly, routing keys may be used for query parameters.
		 * For example the expression /users?id={userId} would match /users?id=bob and /users?id=billy.
		 *
		 * @param expression The routing expression.
		 * @return A reference to the routing configuration.
		 */
		Configuration& Configure(const std::string& expression) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _configurations[expression];
		}

		/**
		 * Deletes this web request router.
		 */
		virtual ~Router() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

};

}

#endif /* REST_HPP_ */
