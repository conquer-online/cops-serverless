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

#ifndef ZFSERVER_CLIENT_H
#define ZFSERVER_CLIENT_H

#include "connection.h"
#include "hook.h"
#include "player.h"

#include <windows.h>
#include <winsock2.h>

#include <cstdint>
#include <atomic>

namespace zfserver
{
	namespace winsock
	{
		// reset the error code of the last socket operation
		void resetLastError(SOCKET socket) noexcept;

		// set the error code of the last socket operation
		void setLastError(int error) noexcept;

		// get the error code of the last socket operation
		int getLastError() noexcept;

		// get the last socket used
		SOCKET getLastUsedSocket() noexcept;
	}

	class Client
	{
	public:
		static constexpr uint16_t ACCSERVER_PORT = 9958;
		static constexpr uint16_t MSGSERVER_PORT = 5816;

	public:
		static Client& instance();

	public:
		void initialize();
		void uninitialize();

		Player& player() noexcept;

	private:
		friend int WINAPI onConnect(SOCKET s, const struct sockaddr_in* name, int namelen);
		friend int WINAPI onSend(SOCKET s, const char* buf, int len, int flags);
		friend int WINAPI onRecv(SOCKET s, char* buf, int len, int flags);
		friend int WINAPI onClose(SOCKET s);
		friend int WINAPI onGetLastError();

		Connection* findConnection(SOCKET socket) noexcept;

		void connect(ConnectionType connectionType, SOCKET socket);
		int processOutgoing(Connection& connection, const char* buf, int len, int flags);
		int processIncoming(Connection& connection, char* buf, int len, int flags);

		int realConnect(SOCKET s, const struct sockaddr_in* name, int namelen);
		int realSend(SOCKET s, const char* buf, int len, int flags);
		int realRecv(SOCKET s, char* buf, int len, int flags);
		int realClose(SOCKET s);
		int realLastError();

	private:
		Client() = default;

	private:
		static std::atomic<Client*> s_instance;

		Hook m_connectHook;
		Hook m_sendHook;
		Hook m_recvHook;
		Hook m_closeHook;
		Hook m_lastErrorHook;

		Connection m_connections[2] = {};
		Player m_player = {}; // create a default dummy player for now...
	};
}

#endif // ZFSERVER_CLIENT_H