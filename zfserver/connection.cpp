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

#include "connection.h"
#include "client.h"

#include "network/msg.h"

#include <numeric>

namespace zfserver
{
	ConnectionType Connection::type() const noexcept
	{
		return m_type;
	}

	SOCKET Connection::socket() const noexcept
	{
		return m_socket;
	}

	security::TqCipher& Connection::cipher() noexcept
	{
		return m_cipher;
	}

	void Connection::connect(ConnectionType type, SOCKET socket) noexcept
	{
		m_type = type;
		m_socket = socket;
		m_cipher = {}; // reset the cipher
	}

	void Connection::sendTo(network::Msg&& msg)
	{
		m_messages.push_back(std::make_unique<network::Msg>(std::move(msg)));
	}

	void Connection::sendTo(const network::Msg& msg)
	{
		m_messages.push_back(std::make_unique<network::Msg>(msg));
	}

	void Connection::sendTo(std::unique_ptr<network::Msg> msg)
	{
		m_messages.push_back(std::move(msg));
	}

	int Connection::recvFrom(char* buf, int len, int flags)
	{
		if (flags == MSG_PEEK)
		{
			int available = std::accumulate(m_messages.cbegin(), m_messages.cend(), 0, [](int available, const auto& msg) { return available + msg->length(); });

			// cannot return 0 - in TCP, it means the remote has gracefully closed the connection
			if (available == 0)
			{
				winsock::setLastError(WSAEWOULDBLOCK);
				return SOCKET_ERROR;
			}

			return available;
		}

		int receivedLength = 0;

		size_t length = 0;
		for (int offset = 0; !m_messages.empty() && offset < len; offset += length)
		{
			auto& msg = m_messages.front();
			length = msg->length();

			// check if next message can fit
			if (offset + length >= len)
				break;

			std::memcpy(buf + offset, msg->buffer(), msg->length());
			receivedLength += length;

			m_messages.pop_front();
		}

		m_cipher.encrypt(reinterpret_cast<uint8_t*>(buf), receivedLength);

		// cannot return 0 - in TCP, it means the remote has gracefully closed the connection
		if (receivedLength == 0)
		{
			winsock::setLastError(WSAEWOULDBLOCK);
			return SOCKET_ERROR;
		}

		return receivedLength;
	}

	void Connection::disconnect() noexcept
	{
		m_type = ConnectionType::Unknown;
		m_socket = INVALID_SOCKET;
	}
}