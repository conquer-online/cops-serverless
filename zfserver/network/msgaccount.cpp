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

#include "msgaccount.h"

#include "client.h"
#include "connection.h"
#include "log.h"

#include "network/msgconnectex.h"

#include "security/rc5.h"

#include <cassert>

using namespace std::literals;

namespace zfserver::network
{
	MsgAccount::MsgAccount(const uint8_t* buf, const size_t len)
		: Msg(buf, len), m_info(bufferAs<MsgInfo>())
	{
		assert(len >= sizeof(MsgInfo));
	}

	MsgAccount::MsgAccount(MsgAccount&& other) noexcept
		: Msg(std::move(other)), m_info(bufferAs<MsgInfo>())
	{

	}

	MsgAccount::MsgAccount(const MsgAccount& other)
		: Msg(other), m_info(bufferAs<MsgInfo>())
	{

	}

	MsgAccount& MsgAccount::operator=(MsgAccount&& other) noexcept
	{
		Msg::operator=(std::move(other));
		m_info = bufferAs<MsgInfo>();

		return *this;
	}

	MsgAccount& MsgAccount::operator=(const MsgAccount& other)
	{
		Msg::operator=(other);
		m_info = bufferAs<MsgInfo>();

		return *this;
	}

	void MsgAccount::process(Client& client, Connection& connection)
	{
		static constexpr uint8_t RC5_SEED[security::RC5::KEY_SIZE] = { 0x3C, 0xDC, 0xFE, 0xE8, 0xC4, 0x54, 0xD6, 0x7E, 0x16, 0xA6, 0xF8, 0x1A, 0xE8, 0xD0, 0x38, 0xBE };

		// these could mean something in the serverless mode, for now just hard-coded...
		static constexpr int32_t ACCOUNT_UID = 123456789;
		static constexpr int32_t ACCOUNT_TOKEN = 987654321;

		security::RC5 cipher{ RC5_SEED };
		cipher.decrypt(reinterpret_cast<uint8_t*>(m_info->Password), sizeof(m_info->Password));

		LOG(DBG, "Requesting login for %s with password %s on %s",
			m_info->Account, m_info->Password, m_info->Server);

		connection.sendTo(MsgConnectEx{ ACCOUNT_UID, ACCOUNT_TOKEN, "192.0.2.1"sv, Client::MSGSERVER_PORT });
	}
}