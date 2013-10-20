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

#ifndef JABBERCLIENT_HPP_
#define JABBERCLIENT_HPP_

#include "../StackTrace.hpp"
#include "../Random.hpp"
#include "../state/StateMachine.hpp"
#include "../net/SslClient.hpp"
#include "../encoding/Base64.hpp"
#include "../xml/Xml.hpp"

namespace nitrus {

class JabberClient : public SslClient {
public:
	typedef std::string JabberId;
	typedef std::string JabberPresence;
	typedef std::string JabberMessage;

public:
	class ClientConnectedEventArgs : public EventArgs {
	public:
		ClientConnectedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		virtual ~ClientConnectedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	typedef EventHandler<const ClientConnectedEventArgs&> ClientConnectedEventHandler;
	typedef Event<const ClientConnectedEventArgs&> ClientConnectedEvent;

	class PresenceReceivedEventArgs : public EventArgs {
	private:
		JabberId _from;
		JabberPresence _presence;

	public:
		PresenceReceivedEventArgs(const JabberId& from, const JabberPresence& presence) : _from(from), _presence(presence) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		const JabberId& From() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _from;
		}

		const JabberPresence& Presence() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _presence;
		}

		virtual ~PresenceReceivedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	typedef EventHandler<const PresenceReceivedEventArgs&> PresenceReceivedEventHandler;
	typedef Event<const PresenceReceivedEventArgs&> PresenceReceivedEvent;

	class MessageReceivedEventArgs : public EventArgs {
	private:
		JabberId _from;
		JabberMessage _message;

	public:
		MessageReceivedEventArgs(const JabberId& from, const JabberMessage& message) : _from(from), _message(message) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}

		const JabberId& From() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _from;
		}

		const JabberMessage& Message() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _message;
		}

		virtual ~MessageReceivedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	typedef EventHandler<const MessageReceivedEventArgs&> MessageReceivedEventHandler;
	typedef Event<const MessageReceivedEventArgs&> MessageReceivedEvent;

private:
	enum State {
		State_Idle,
		State_Connecting,
		State_CanDisconnect,
		State_Connected,
		State_Disconnected,
		State_CreatingPreLoginStream,
		State_CreatedPreLoginStream,
		State_LoggingIn,
		State_LoggedIn,
		State_LoginInvalidAccount,
		State_LoginInvalidPassword,
		State_CreatingAccount,
		State_CreatingPostLoginStream,
		State_CreatedPostLoginStream,
		State_BindingResource,
		State_BoundResource,
		State_CreatingSession,
		State_InSession,
		State_CreatedSession
	};

	enum Trigger {
		Trigger_Connect,
		Trigger_Connected,
		Trigger_Disconnected,
		Trigger_CreateStream,
		Trigger_CreatedStream,
		Trigger_Login,
		Trigger_LoggedIn,
		Trigger_LoginInvalidAccount,
		Trigger_LoginInvalidPassword,
		Trigger_InvalidLogin,
		Trigger_CreateAccount,
		Trigger_CreatedAccount,
		Trigger_BindResource,
		Trigger_BoundResource,
		Trigger_CreateSession,
		Trigger_CreatedSession
	};

private:
	std::string _username;
	std::string _password;
	StateMachine<State, Trigger> _stateMachine;
	Xml::ProgressiveParser _progressiveParser;
	Xml::DocumentParser _documentParser;
	Socket::Endpoint _serverEndpoint;
	JabberId _jid;
	ClientConnectedEvent _clientConnected;
	PresenceReceivedEvent _presenceReceived;
	MessageReceivedEvent _messageReceived;

private:
	static std::string PlainAuthentication(const std::string& username, const std::string& password) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		std::string decoded;
		decoded += (char)0;
		decoded += username;
		decoded += (char)0;
		decoded += password;

		return Base64::Encode(decoded);
	}

	static std::string RandomId(const std::string& type) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		static uint64_t id = 0;
		return String::Convert(id++);
	}

	void Send(const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		// Log::Debug("Sending: %s", value.c_str());
		SslClient::Send(value);
	}

	void CreateStream() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_stateMachine.Fire(Trigger_CreateStream);

		XmlElement xml("stream", "stream");
		xml.Attribute("to").Value(_serverEndpoint.Address());
		xml.Attribute("xmlns").Value("jabber:client");
		xml.Attribute("xmlns", "stream").Value("http://etherx.jabber.org/streams");
		xml.Attribute("version").Value("1.0");

		Send(xml.ToStartElementString());
	}

	void Login(const std::string& username, const std::string& password) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_stateMachine.Fire(Trigger_Login);

		XmlElement xml("auth");
		xml.Attribute("xmlns").Value("urn:ietf:params:xml:ns:xmpp-sasl");
		xml.Attribute("mechanism").Value("PLAIN");
		xml.Value(PlainAuthentication(username, password));

		Send(xml.ToString());
	}

	void CreateAccount(const std::string& username, const std::string& password) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_stateMachine.Fire(Trigger_CreateAccount);

		XmlElement xml("iq");
		xml.Attribute("type").Value("set");
		xml.Attribute("id").Value(RandomId("register"));
		xml.Element("query").Attribute("xmlns").Value("jabber:iq:register");
		xml.Element("query").Element("username").Value(username);
		xml.Element("query").Element("password").Value(password);

		Send(xml.ToString());
	}

	void BindResource() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_stateMachine.Fire(Trigger_BindResource);

		XmlElement xml("iq");
		xml.Attribute("to").Value(_serverEndpoint.Address());
		xml.Attribute("type").Value("set");
		xml.Attribute("id").Value(RandomId("bind"));
		xml.Element("bind").Attribute("xmlns").Value("urn:ietf:params:xml:ns:xmpp-bind");

		Send(xml.ToString());
	}

	void CreateSession() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_stateMachine.Fire(Trigger_CreateSession);

		XmlElement xml("iq");
		xml.Attribute("to").Value(_serverEndpoint.Address());
		xml.Attribute("type").Value("set");
		xml.Attribute("id").Value(RandomId("session"));
		xml.Element("session").Attribute("xmlns").Value("urn:ietf:params:xml:ns:xmpp-session");

		Send(xml.ToString());
	}

	void Presence() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Send(XmlElement("presence").ToString());
	}

	void Presence(const std::string to, const std::string& type) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		XmlElement xml("presence");
		xml.Attribute("from").Value(_jid);
		xml.Attribute("to").Value(to);
		xml.Attribute("type").Value(type);

		Send(xml.ToString());
	}

	void AcceptFriendRequest(const std::string& jid, const std::string& id = RandomId("iq")) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		XmlElement xml("iq");
		xml.Attribute("type").Value("set");
		xml.Attribute("id").Value(id);
		xml.Element("query").Attribute("xmlns").Value("jabber:iq:roster");
		xml.Element("query").Element("item").Attribute("jid").Value(jid);
		xml.Element("query").Element("item").Attribute("subscription").Value("to");

		Send(xml.ToString());
	}

	void KeepAlive() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Send(" "); // http://tools.ietf.org/html/rfc6120#section-4.6
		Thread::SetTimeout(TimeSpan::FromMinutes(1), delegate(&JabberClient::KeepAlive, this));
	}

private:
	template <typename Signature> friend class Delegate;

	void OnClientConnected(const SslClient::ClientConnectedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_stateMachine.Fire(Trigger_Connected);
	}

	void State_Connected_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		CreateStream();
	}

	void State_CreatedPreLoginStream_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Login(_username, _password);
	}

	void State_LoginInvalidAccount_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		CreateAccount(_username, _password);
	}

	void State_LoginInvalidPassword_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Log::Error("Invalid password");
	}

	void State_LoggedIn_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		CreateStream();
	}

	void State_CreatedPostLoginStream_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		BindResource();
	}

	void State_BoundResource_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		CreateSession();
	}

	void State_CreatedSession_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_clientConnected(ClientConnectedEventArgs(), this);
		Presence();
		KeepAlive();
	}

	void OnDataReceived(const SslClient::DataReceivedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Log::Debug("Received: %s", args.Data().c_str());
		
		if (_stateMachine.State() == State_CreatingPreLoginStream || _stateMachine.State() == State_CreatingPostLoginStream) {
			_progressiveParser.Append(args.Data());
		}
		else {
			_documentParser.Append(args.Data());
		}
	}

	void OnClientDisconnected(const SslClient::ClientDisconnectedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_stateMachine.Fire(Trigger_Disconnected);
	}

	void OnStartElementReceived(const Xml::ProgressiveParser::StartElementReceivedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (args.Namespace() == "stream" && args.Name() == "stream") {
			_documentParser.Append(_progressiveParser.Buffer().substr(_progressiveParser.Buffer().find('>') + 1));
			_stateMachine.Fire(Trigger_CreatedStream);
		}
	}

	void OnDocumentParsed(const Xml::DocumentParser::DocumentParsedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		if (_stateMachine.State() == State_LoggingIn && args.Document().Name() == "success" && args.Document().Attribute("xmlns").Value() == "urn:ietf:params:xml:ns:xmpp-sasl") {
			_stateMachine.Fire(Trigger_LoggedIn);
		}
		else if (_stateMachine.State() == State_LoggingIn && args.Document().Name() == "failure" && args.Document().Attribute("xmlns").Value() == "urn:ietf:params:xml:ns:xmpp-sasl" && args.Document().Element("bad-protocol").Exists()) {
			_stateMachine.Fire(Trigger_LoginInvalidAccount);
		}
		else if (_stateMachine.State() == State_LoggingIn && args.Document().Name() == "failure" && args.Document().Attribute("xmlns").Value() == "urn:ietf:params:xml:ns:xmpp-sasl" && args.Document().Element("not-authorized").Exists()) {
			_stateMachine.Fire(Trigger_LoginInvalidPassword);
		}
		else if (_stateMachine.State() == State_CreatingAccount && args.Document().Name() == "iq" && args.Document().Element("query").Attribute("xmlns").Value() == "jabber:iq:register") {
			_stateMachine.Fire(Trigger_CreatedAccount);
		}
		else if (_stateMachine.State() == State_BindingResource && args.Document().Name() == "iq" && args.Document().Element("bind").Attribute("xmlns").Value() == "urn:ietf:params:xml:ns:xmpp-bind") {
			_jid = args.Document().Element("bind").Element("jid").Value();
			_stateMachine.Fire(Trigger_BoundResource);
		}
		else if (_stateMachine.State() == State_CreatingSession && args.Document().Name() == "iq" && args.Document().Element("session").Attribute("xmlns").Value() == "urn:ietf:params:xml:ns:xmpp-session") {
			_stateMachine.Fire(Trigger_CreatedSession);
		}
		else if (args.Document().Name() == "iq" && args.Document().Attribute("type").Value() == "set" && args.Document().Element("query").Element("item").Attribute("subscription").Value() == "from") {
			AcceptFriendRequest(args.Document().Element("query").Element("item").Attribute("jid").Value(), args.Document().Attribute("id").Value());
		}
		else if (args.Document().Name() == "presence" && args.Document().Attribute("type").Value() == "subscribe") {
			Presence(args.Document().Attribute("from").Value(), "subscribed");
		}
		else if (args.Document().Name() == "presence" && args.Document().Element("show").Value().empty() == false) {
			_presenceReceived(PresenceReceivedEventArgs(args.Document().Attribute("from").Value(), args.Document().Element("show").Value()), this);
		}
		else if (args.Document().Name() == "message") {
			_messageReceived(MessageReceivedEventArgs(args.Document().Attribute("from").Value(), args.Document().Element("body").Value()), this);
		}
		else {
			Log::Warning("Unhandled document: %s", args.Document().ToString().c_str());
		}
	}

public:
	JabberClient(const std::string& username, const std::string& password) : _username(username), _password(password), _stateMachine(State_Idle), _progressiveParser(), _documentParser(), _serverEndpoint(), _jid(), _clientConnected(), _presenceReceived(), _messageReceived() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		SslClient::ClientConnected() += delegate(&JabberClient::OnClientConnected, this);
		SslClient::DataReceived() += delegate(&JabberClient::OnDataReceived, this);
		SslClient::ClientDisconnected() += delegate(&JabberClient::OnClientDisconnected, this);

		_progressiveParser.StartElementReceived() += delegate(&JabberClient::OnStartElementReceived, this);
		_documentParser.DocumentParsed() += delegate(&JabberClient::OnDocumentParsed, this);

		_stateMachine.Configure(State_Idle)
			.Permit(Trigger_Connect, State_Connecting);

		_stateMachine.Configure(State_Connecting)
			.Permit(Trigger_Connected, State_Connected);

		_stateMachine.Configure(State_CanDisconnect)
			.Permit(Trigger_Disconnected, State_Disconnected);

		_stateMachine.Configure(State_Connected)
			.SubstateOf(State_CanDisconnect)
			.OnEntry(delegate(&JabberClient::State_Connected_Entered, this))
			.Permit(Trigger_CreateStream, State_CreatingPreLoginStream);

		_stateMachine.Configure(State_CreatingPreLoginStream)
			.SubstateOf(State_CanDisconnect)
			.Permit(Trigger_CreatedStream, State_CreatedPreLoginStream);

		_stateMachine.Configure(State_CreatedPreLoginStream)
			.SubstateOf(State_CanDisconnect)
			.OnEntry(delegate(&JabberClient::State_CreatedPreLoginStream_Entered, this))
			.Permit(Trigger_Login, State_LoggingIn);

		_stateMachine.Configure(State_LoggingIn)
			.SubstateOf(State_CanDisconnect)
			.Permit(Trigger_InvalidLogin, State_CreatedPreLoginStream)
			.Permit(Trigger_LoggedIn, State_LoggedIn)
			.Permit(Trigger_LoginInvalidAccount, State_LoginInvalidAccount)
			.Permit(Trigger_LoginInvalidPassword, State_LoginInvalidPassword);

		_stateMachine.Configure(State_LoginInvalidAccount)
			.SubstateOf(State_CanDisconnect)
			.OnEntry(delegate(&JabberClient::State_LoginInvalidAccount_Entered, this))
			.Permit(Trigger_InvalidLogin, State_CreatedPreLoginStream)
			.Permit(Trigger_CreateAccount, State_CreatingAccount);

		_stateMachine.Configure(State_LoginInvalidPassword)
			.SubstateOf(State_CanDisconnect)
			.OnEntry(delegate(&JabberClient::State_LoginInvalidPassword_Entered, this))
			.Permit(Trigger_InvalidLogin, State_CreatedPreLoginStream)
			.Permit(Trigger_Login, State_LoggingIn);

		_stateMachine.Configure(State_CreatingAccount)
			.SubstateOf(State_CanDisconnect)
			.Permit(Trigger_CreatedAccount, State_LoggedIn);

		_stateMachine.Configure(State_LoggedIn)
			.SubstateOf(State_CanDisconnect)
			.OnEntry(delegate(&JabberClient::State_LoggedIn_Entered, this))
			.Permit(Trigger_CreateStream, State_CreatingPostLoginStream);

		_stateMachine.Configure(State_CreatingPostLoginStream)
			.SubstateOf(State_CanDisconnect)
			.Permit(Trigger_CreatedStream, State_CreatedPostLoginStream);

		_stateMachine.Configure(State_CreatedPostLoginStream)
			.SubstateOf(State_CanDisconnect)
			.OnEntry(delegate(&JabberClient::State_CreatedPostLoginStream_Entered, this))
			.Permit(Trigger_BindResource, State_BindingResource);

		_stateMachine.Configure(State_BindingResource)
			.SubstateOf(State_CanDisconnect)
			.Permit(Trigger_BoundResource, State_BoundResource);

		_stateMachine.Configure(State_BoundResource)
			.SubstateOf(State_CanDisconnect)
			.OnEntry(delegate(&JabberClient::State_BoundResource_Entered, this))
			.Permit(Trigger_CreateSession, State_CreatingSession);

		_stateMachine.Configure(State_CreatingSession)
			.SubstateOf(State_CanDisconnect)
			.Permit(Trigger_CreatedSession, State_CreatedSession);

		_stateMachine.Configure(State_InSession)
			.SubstateOf(State_CanDisconnect);

		_stateMachine.Configure(State_CreatedSession)
			.SubstateOf(State_InSession)
			.OnEntry(delegate(&JabberClient::State_CreatedSession_Entered, this));

		_stateMachine.Configure(State_Disconnected);
	}

	ClientConnectedEvent& ClientConnected() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _clientConnected;
	}

	PresenceReceivedEvent& PresenceReceived() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _presenceReceived;
	}

	MessageReceivedEvent& MessageReceived() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _messageReceived;
	}

	const JabberId& Id() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _jid;
	}

	void Connect(const Endpoint& endpoint) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_stateMachine.Fire(Trigger_Connect);
		_serverEndpoint = endpoint;
		SslClient::Connect(endpoint);
	}

	void Message(const JabberId& to, const JabberMessage& message) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		XmlElement xml("message");
		xml.Attribute("from").Value(_jid);
		xml.Attribute("to").Value(to);
		xml.Element("body").Value(message);

		Send(xml.ToString());
	}

	virtual ~JabberClient() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}
};

}

#endif /* HTTPCLIENT_HPP_ */
