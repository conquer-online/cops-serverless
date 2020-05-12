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

#include "msgconnectex.h"

#include <cassert>

namespace zfserver::network
{
	MsgConnectEx::MsgConnectEx(const int32_t accountUID, const int32_t data, std::string_view info, const uint16_t port)
		: Msg(sizeof(MsgInfo)), m_info(bufferAs<MsgInfo>())
	{
		create(accountUID, data, info, port);
	}

	MsgConnectEx::MsgConnectEx(MsgConnectEx&& other) noexcept
		: Msg(std::move(other)), m_info(bufferAs<MsgInfo>())
	{

	}

	MsgConnectEx::MsgConnectEx(const MsgConnectEx& other)
		: Msg(other), m_info(bufferAs<MsgInfo>())
	{

	}

	MsgConnectEx& MsgConnectEx::operator=(MsgConnectEx&& other) noexcept
	{
		Msg::operator=(std::move(other));
		m_info = bufferAs<MsgInfo>();

		return *this;
	}

	MsgConnectEx& MsgConnectEx::operator=(const MsgConnectEx& other)
	{
		Msg::operator=(other);
		m_info = bufferAs<MsgInfo>();

		return *this;
	}

	void MsgConnectEx::create(const int32_t accountUID, const int32_t data, std::string_view info, const uint16_t port)
	{
		assert(info.size() < MAX_NAMESIZE);

		m_info->Header.Length = static_cast<uint16_t>(length());
		m_info->Header.Type = MSG_CONNECTEX;

		m_info->AccountUID = accountUID;
		m_info->Data = data;
		std::memcpy(m_info->Info, info.data(), info.size());
		m_info->Port = port;
	}
}