//
//  Copyright (c) CptSky <cptsky@me.com>
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the organization nor the names of its contributors
//       may be used to endorse or promote products derived from this software
//       without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "client.h"

#include "log.h"
#include "network/msg.h"

#include <cassert>
#include <algorithm>

 // ensure we link ws2_32 (even if not specified in the link flags)
#pragma comment(lib, "ws2_32.lib")

namespace zfserver
{
	// forward declarations
	int WINAPI onConnect(SOCKET s, const struct sockaddr_in* name, int namelen);
	int WINAPI onSend(SOCKET s, const char* buf, int len, int flags);
	int WINAPI onRecv(SOCKET s, char* buf, int len, int flags);
	int WINAPI onClose(SOCKET s);
	int WINAPI onGetLastError();

	std::atomic<Client*> Client::s_instance = { nullptr };

	Client& Client::instance()
	{
		if (s_instance == nullptr)
		{
			static std::atomic<bool> creating = { false };
			if (!creating.exchange(true))
			{
				// create the instance...
				s_instance = new Client();
			}
			else
			{
				// wait for the instance
				while (s_instance == nullptr) Sleep(0);
			}
		}

		return *s_instance;
	}

	void Client::initialize()
	{
		LOG(VRB, "Initializing...");

		m_connectHook.redirect(GetProcAddress(GetModuleHandleA("ws2_32.dll"), "connect"), &onConnect);
		m_sendHook.redirect(GetProcAddress(GetModuleHandleA("ws2_32.dll"), "send"), &onSend);
		m_recvHook.redirect(GetProcAddress(GetModuleHandleA("ws2_32.dll"), "recv"), &onRecv);
		m_closeHook.redirect(GetProcAddress(GetModuleHandleA("ws2_32.dll"), "closesocket"), &onClose);
		m_lastErrorHook.redirect(GetProcAddress(GetModuleHandleA("ws2_32.dll"), "WSAGetLastError"), &onGetLastError);

		LOG(VRB, "Initialization done... Hooked networking functions...");
	}

	void Client::uninitialize()
	{

	}

	Player& Client::player() noexcept
	{
		return m_player;
	}

	Connection* Client::findConnection(SOCKET socket) noexcept
	{
		auto connectionIt = std::find_if(std::begin(m_connections), std::end(m_connections), [socket](const auto& connection) { return connection.socket() == socket; });
		return connectionIt != std::end(m_connections) ? &(*connectionIt) : nullptr;
	}

	void Client::connect(ConnectionType connectionType, SOCKET socket)
	{
		int index = static_cast<int>(connectionType);
		m_connections[index].connect(connectionType, socket);
	}

	int Client::processOutgoing(Connection& connection, const char* buf, int len, int flags)
	{
		auto& cipher = connection.cipher();

		uint8_t data[4192]; // should be large enough for Conquer Online 2.0
		std::memcpy(data, buf, len); // we could modify buf directly, but that would violate the send contract

		cipher.decrypt(data, len);

		uint16_t length = 0;
		for (int offset = 0; offset < len; offset += length)
		{
			network::Msg::Header& header = *(network::Msg::Header*)(data + offset);
			LOG(DBG, "Client sent %u (%u) on socket %u", header.Type, header.Length, socket);

			length = header.Length;
			assert(offset + length <= len);

			auto msg = network::Msg::create(data + offset, header.Length);
			assert(msg != nullptr);

			msg->process(*this, connection);
		}

		return len; // fully processed
	}

	int Client::processIncoming(Connection& connection, char* buf, int len, int flags)
	{
		return connection.recvFrom(buf, len, flags);
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Real calls
	/////////////////////////////////////////////////////////////////////////////////
	int Client::realConnect(SOCKET s, const struct sockaddr_in* name, int namelen)
	{
		LOG(VRB, "Calling connect(%u, %s:%u, %d)", s, inet_ntoa(name->sin_addr), ntohs(name->sin_port), namelen);

		using LPFCONNECT = int (WINAPI*)(SOCKET, const sockaddr_in*, int);
		return ((LPFCONNECT)m_connectHook.thunk())(s, name, namelen);
	}

	int Client::realSend(SOCKET s, const char* buf, int len, int flags)
	{
		LOG(VRB, "Calling send(%u, %p, %d, %d)", s, buf, len, flags);

		using  LPFSEND = int (WINAPI*)(SOCKET, const char*, int, int);
		return ((LPFSEND)m_sendHook.thunk())(s, buf, len, flags);
	}

	int Client::realRecv(SOCKET s, char* buf, int len, int flags)
	{
		LOG(VRB, "Calling recv(%u, %p, %d, %d)", s, buf, len, flags);

		using LPFRECV = int (WINAPI*)(SOCKET, char*, int, int);
		return ((LPFRECV)m_recvHook.thunk())(s, buf, len, flags);
	}

	int Client::realClose(SOCKET s)
	{
		LOG(VRB, "Calling closesocket(%u)", s);

		using LPFCLOSESOCKET = int (WINAPI*)(SOCKET);
		return ((LPFCLOSESOCKET)m_closeHook.thunk())(s);
	}

	int Client::realLastError()
	{
		LOG(VRB, "Calling WSAGetLastError()");

		using LPFWSAGETLASTERROR = int (WINAPI*)();
		return ((LPFWSAGETLASTERROR)m_lastErrorHook.thunk())();
	}

	/////////////////////////////////////////////////////////////////////////////////
	// Hooks
	/////////////////////////////////////////////////////////////////////////////////
	namespace winsock
	{
		thread_local SOCKET g_lastSocket = INVALID_SOCKET;
		thread_local int g_lastSocketError = 0;

		void resetLastError(SOCKET socket) noexcept
		{
			g_lastSocket = socket;
			g_lastSocketError = 0;
		}

		void setLastError(int error) noexcept
		{
			g_lastSocketError = error;
		}

		int getLastError() noexcept
		{
			return g_lastSocketError;
		}

		SOCKET getLastUsedSocket() noexcept
		{
			return g_lastSocket;
		}
	}

	int WINAPI onConnect(SOCKET s, const struct sockaddr_in* name, int namelen)
	{
		static Client& client = Client::instance();

		// reset last error
		winsock::resetLastError(s);

		const char* address = inet_ntoa(name->sin_addr);
		const uint16_t port = ntohs(name->sin_port);
		LOG(VRB, "Intercepted connect(%s:%u) call for socket %u.", address, port, s);

		if (port == Client::ACCSERVER_PORT)
		{
			LOG(DBG, "Mocking the AccServer");
			client.connect(ConnectionType::AccServer, s);
			return 0; // success
		}
		else if (port == Client::MSGSERVER_PORT)
		{
			LOG(DBG, "Mocking the MsgServer");
			client.connect(ConnectionType::MsgServer, s);
			return 0; // success
		}
		else
		{
			// connect the socket -- external connection
			return client.realConnect(s, name, namelen);
		}
	}

	int WINAPI onSend(SOCKET s, const char* buf, int len, int flags)
	{
		static Client& client = Client::instance();

		// reset last error
		winsock::resetLastError(s);

		LOG(VRB, "Intercepted send(%p, %d, %d) call for socket %u.", buf, len, flags, s);

		Connection* connection = client.findConnection(s);
		return connection != nullptr ? client.processOutgoing(*connection, buf, len, flags) : client.realSend(s, buf, len, flags);
	}

	int WINAPI onRecv(SOCKET s, char* buf, int len, int flags)
	{
		static Client& client = Client::instance();

		LOG(VRB, "Intercepted recv(%p, %d, %d) call for socket %u.", buf, len, flags, s);

		Connection* connection = client.findConnection(s);
		return connection != nullptr ? client.processIncoming(*connection, buf, len, flags) : client.realRecv(s, buf, len, flags);
	}

	int WINAPI onClose(SOCKET s)
	{
		static Client& client = Client::instance();

		// reset last error
		winsock::resetLastError(s);

		LOG(VRB, "Intercepted closesocket() call for socket %u.", s);

		Connection* connection = client.findConnection(s);
		if (connection != nullptr)
		{
			LOG(DBG, "Disconnecting the mocked %s.", connection->type() == ConnectionType::MsgServer ? "MsgServer" : "AccServer");
			connection->disconnect();

			// still need to close the actual socket descriptor that was created (but never connected)
			return client.realClose(s);
		}
		else
		{
			// not the fake socket
			return client.realClose(s);
		}
	}

	int WINAPI onGetLastError()
	{
		static Client& client = Client::instance();

		LOG(VRB, "Intercepted WSAGetLastError() call.");

		Connection* connection = client.findConnection(winsock::getLastUsedSocket());
		return connection != nullptr ? winsock::getLastError() : client.realLastError();
	}
}