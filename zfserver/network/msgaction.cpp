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

#include "msgaction.h"

#include "client.h"
#include "connection.h"
#include "player.h"
#include "log.h"

#include <cassert>

namespace zfserver::network
{
	MsgAction::MsgAction(const uint8_t* buf, const size_t len)
		: Msg(buf, len), m_info(bufferAs<MsgInfo>())
	{
		assert(len >= sizeof(MsgInfo));
	}

	MsgAction::MsgAction(MsgAction&& other) noexcept
		: Msg(std::move(other)), m_info(bufferAs<MsgInfo>())
	{

	}

	MsgAction::MsgAction(const MsgAction& other)
		: Msg(other), m_info(bufferAs<MsgInfo>())
	{

	}

	MsgAction& MsgAction::operator=(MsgAction&& other) noexcept
	{
		Msg::operator=(std::move(other));
		m_info = bufferAs<MsgInfo>();

		return *this;
	}

	MsgAction& MsgAction::operator=(const MsgAction& other)
	{
		Msg::operator=(other);
		m_info = bufferAs<MsgInfo>();

		return *this;
	}

	void MsgAction::process(Client& client, Connection& connection)
	{
		auto& player = client.player();

		switch (m_info->Action)
		{
		case Action::EnterMap: // Login Sequence - Part 1
		{
			assert(m_info->UniqId == player.uid());

			m_info->PosX = 400;
			m_info->PosY = 400;
			m_info->Data = 1002;
			m_info->Direction = 0;

			connection.sendTo(*this);
			break;
		}
		case Action::GetItems: // Login Sequence - Part 2
		{
			assert(m_info->UniqId == player.uid());
			connection.sendTo(*this);
			break;
		}
		case Action::GetFriends: // Login Sequence - Part 3
		{
			assert(m_info->UniqId == player.uid());
			connection.sendTo(*this);
			break;
		}
		case Action::GetWeaponSkills: // Login Sequence - Part 4
		{
			assert(m_info->UniqId == player.uid());
			connection.sendTo(*this);
			break;
		}
		case Action::GetMagicSkills: // Login Sequence - Part 5
		{
			assert(m_info->UniqId == player.uid());
			connection.sendTo(*this);
			break;
		}
		case Action::GetSyndicate: // Login Sequence - Part 6
		{
			assert(m_info->UniqId == player.uid());
			connection.sendTo(*this);
			break;
		}
		case Action::CompleteLogin: // Login Sequence - Part 7
		{
			assert(m_info->UniqId == player.uid());
			break;
		}
		default:
			LOG(WARN, "Unknown action[%04u], data=[%d]", m_info->Action, m_info->Data);
			break;
		}
	}
}