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

#include "../include/Application.hpp"
#include "../include/http/HttpClient.hpp"
using namespace nitrus;

void OnResponseStarted(const HttpClient::ResponseStartedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	HttpClient* client = static_cast<HttpClient*>(sender);
	Log::Debug("OnResponseStarted (%s, %d, %s)", args.Protocol().c_str(), args.Code(), args.Description().c_str());
}

void OnHeaderReceived(const HttpClient::HeaderReceivedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	HttpClient* client = static_cast<HttpClient*>(sender);
	Log::Debug("OnHeaderReceived (%s, %s)", args.Key().c_str(), args.Value().c_str());
}

void OnContentReceived(const HttpClient::ContentReceivedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	HttpClient* client = static_cast<HttpClient*>(sender);
	Log::Debug("OnContentReceived (%d)", args.Content().length());
}

void OnResponseEnded(const HttpClient::ResponseEndedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	HttpClient* client = static_cast<HttpClient*>(sender);
	Log::Debug("OnResponseEnded");
}

void OnClientConnected(const TcpClient::ClientConnectedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	HttpClient* client = static_cast<HttpClient*>(sender);

	client->ResponseStarted() += delegate(OnResponseStarted);
	client->HeaderReceived() += delegate(OnHeaderReceived);
	client->ContentReceived() += delegate(OnContentReceived);
	client->ResponseEnded() += delegate(OnResponseEnded);

	client->Begin("GET", Application::GetParameter("--path", "/"), "HTTP/1.1").SendHeader("Host", Application::GetParameter("--host", "localhost")).SendHeader("Connection", "close").Send("").End();
}

/**
 * The entry point for the application.
 * @param argc The number of elements in the second parameter.
 * @param argv The array of arguments passed to this application from the system.
 * @return EXIT_SUCCESS if the application completed successfully or EXIT_FAILURE if an error occurred.
 */
int main(int argc, char** argv) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	Application::Initialize(argc, argv);
	HttpClient client;
	client.ClientConnected() += delegate(OnClientConnected);
	client.Connect(Socket::Endpoint(Application::GetParameter("--host", "localhost"), Application::GetParameter("--port", 80)));
	return Application::Run();
}
