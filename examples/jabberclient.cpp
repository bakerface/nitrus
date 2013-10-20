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
#include "../include/jabber/JabberClient.hpp"
using namespace nitrus;

void OnClientConnected(const JabberClient::ClientConnectedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	JabberClient* client = static_cast<JabberClient*>(sender);
	Log::Information("Client %s has connected", client->Id().c_str());
}

void OnPresenceReceived(const JabberClient::PresenceReceivedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	JabberClient* client = static_cast<JabberClient*>(sender);
	Log::Information("Presence for %s: %s", args.From().c_str(), args.Presence().c_str());
}

void OnMessageReceived(const JabberClient::MessageReceivedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	JabberClient* client = static_cast<JabberClient*>(sender);
	Log::Information("%s: %s", args.From().c_str(), args.Message().c_str());
	client->Message(args.From(), "hey, I am a jabber bot.");
}

void OnClientDisconnected(const SslClient::ClientDisconnectedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	JabberClient* client = static_cast<JabberClient*>(sender);
	Log::Information("Client has disconnected");
}

/**
 * The entry point for the application.
 * @param argc The number of elements in the second parameter.
 * @param argv The array of arguments passed to this application from the system.
 * @return EXIT_SUCCESS if the application completed successfully or EXIT_FAILURE if an error occurred.
 */
int main(int argc, char** argv) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	Application::Initialize(argc, argv);

	JabberClient client(Application::GetParameter("--username"), Application::GetParameter("--password"));
	client.ClientConnected() += delegate(OnClientConnected);
	client.PresenceReceived() += delegate(OnPresenceReceived);
	client.MessageReceived() += delegate(OnMessageReceived);
	client.ClientDisconnected() += delegate(OnClientDisconnected);
	client.Connect(Socket::Endpoint(Application::GetParameter("--server", "macjabber.com"), Application::GetParameter("--port", 5222)));
	return Application::Run();
}
