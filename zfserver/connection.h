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

#ifndef ZFSERVER_CONNECTION_H
#define ZFSERVER_CONNECTION_H

#include "security/tqcipher.h"

#include <deque>
#include <memory>

#include <winsock2.h>

namespace zfserver
{
	namespace network
	{
		class Msg;
	}

	enum class ConnectionType : int
	{
		Unknown = -1,
		AccServer = 0,
		MsgServer = 1,
	};

	class Connection final
	{
	public:
		Connection() = default;
		~Connection() = default;

		ConnectionType type() const noexcept;
		SOCKET socket() const noexcept;
		security::TqCipher& cipher() noexcept;

		void connect(ConnectionType type, SOCKET socket) noexcept;

		void sendTo(network::Msg&& msg);
		void sendTo(const network::Msg& msg);
		void sendTo(std::unique_ptr<network::Msg> msg);

		int recvFrom(char* buf, int len, int flags);
		
		void disconnect() noexcept;

	private:
		ConnectionType m_type = ConnectionType::Unknown;
		SOCKET m_socket = INVALID_SOCKET;
		security::TqCipher m_cipher = {};
		std::deque<std::unique_ptr<network::Msg>> m_messages = {};
	};
}

#endif // ZFSERVER_CONNECTION_H