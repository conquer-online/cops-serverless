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

#include "msg.h"

#include "client.h"
#include "log.h"

#include "network/msgaccount.h"
#include "network/msgaction.h"
#include "network/msgconnect.h"
#include "network/msgitem.h"
#include "network/msgtalk.h"
#include "network/msgwalk.h"

#include <cassert>
#include <cctype>

namespace zfserver::network
{
	std::unique_ptr<Msg> Msg::create(const uint8_t* buf, const size_t len)
	{
		assert(buf != nullptr);
		assert(len >= sizeof(Msg::Header));

		std::unique_ptr<Msg> msg = nullptr;

		const Msg::Header* header = reinterpret_cast<const Msg::Header*>(buf);
		switch (header->Type)
		{
		case MSG_ACCOUNT:
			msg = std::make_unique<MsgAccount>(buf, len);
			break;
		case MSG_ACTION:
			msg = std::make_unique<MsgAction>(buf, len);
			break;
		case MSG_CONNECT:
			msg = std::make_unique<MsgConnect>(buf, len);
			break;
		case MSG_ITEM:
			msg = std::make_unique<MsgItem>(buf, len);
			break;
		case MSG_TALK:
			msg = std::make_unique<MsgTalk>(buf, len);
			break;
		case MSG_WALK:
			msg = std::make_unique<MsgWalk>(buf, len);
			break;
		default:
			msg = std::make_unique<Msg>(buf, len);
			break;
		}

		return msg;
	}

	Msg::Msg(const uint8_t* buf, const size_t len)
		: m_length(len)
	{
		assert(buf != nullptr);
		assert(len >= sizeof(Msg::Header));

		m_buffer = std::make_unique<uint8_t[]>(m_length);
		std::memcpy(m_buffer.get(), buf, len);
	}

	Msg::Msg(const size_t len)
		: m_length(len)
	{
		m_buffer = std::make_unique<uint8_t[]>(m_length);
		std::memset(m_buffer.get(), 0, m_length);
	}

	Msg::Msg(Msg&& other) noexcept
		: m_buffer(std::move(other.m_buffer)), m_length(other.m_length)
	{
		other.m_buffer = nullptr;
		other.m_length = 0;
	}

	Msg::Msg(const Msg& other)
		: Msg(other.m_length)
	{
		// copy the data
		std::memcpy(m_buffer.get(), other.m_buffer.get(), other.m_length);
	}

	Msg& Msg::operator=(Msg&& other) noexcept
	{
		if (&other != this)
		{
			m_length = 0;
			m_buffer = nullptr;

			std::swap(m_buffer, other.m_buffer);
			std::swap(m_length, other.m_length);
		}

		return *this;
	}

	Msg& Msg::operator=(const Msg& other)
	{
		if (&other != this)
		{
			m_length = other.m_length;
			m_buffer = std::make_unique<uint8_t[]>(m_length);

			// copy the data
			std::memcpy(m_buffer.get(), other.m_buffer.get(), other.m_length);
		}

		return *this;
	}

	void Msg::process(Client& client, Connection& connection)
	{
		const Msg::Header* header = reinterpret_cast<const Msg::Header*>(m_buffer.get());
		LOG(WARN, "Unknown msg[%04d], len=[%03d]", header->Type, header->Length);

		dump(*this);
	}

	void Msg::dump(Msg& msg)
	{
		dump(msg.buffer(), msg.length());
	}

	void Msg::dump(const uint8_t* buf, const size_t len)
	{
		assert(buf != nullptr);

		static constexpr size_t AMOUNT_PER_LINE = 16;

		char dumpedMsg[8192];
		int offset = 0;

		for (size_t i = 0; i < len; i += AMOUNT_PER_LINE)
		{
			// print the hex values
			for (size_t j = 0; j < AMOUNT_PER_LINE; ++j)
			{
				if (i + j < len)
				{
					offset += std::sprintf(dumpedMsg + offset, "%02X ", buf[i + j]);
				}
				else
				{
					offset += std::sprintf(dumpedMsg + offset, "   ");
				}
			}
			offset += std::sprintf(dumpedMsg + offset, " ");

			// print the ASCII characters
			for (size_t j = 0; j < AMOUNT_PER_LINE; ++j)
			{
				if (i + j < len)
				{
					offset += std::sprintf(dumpedMsg + offset, "%c",
						std::isprint(buf[i + j]) ? buf[i + j] : '.');
				}
			}
			offset += std::sprintf(dumpedMsg + offset, "\n");
		}

		LOG(DBG, "%s", dumpedMsg);
	}
}