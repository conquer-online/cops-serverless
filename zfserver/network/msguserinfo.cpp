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

#include "msguserinfo.h"

#include "player.h"

#include "network/stringpacker.h"

#include <cassert>

namespace zfserver::network
{
	MsgUserInfo::MsgUserInfo(const Player& aPlayer)
		: Msg(sizeof(MsgInfo) +
			1 + aPlayer.name().size() +
			1 + aPlayer.mate().size())
		, m_info(bufferAs<MsgInfo>())
	{
		create(aPlayer);
	}

	MsgUserInfo::MsgUserInfo(MsgUserInfo&& other) noexcept
		: Msg(std::move(other)), m_info(bufferAs<MsgInfo>())
	{

	}

	MsgUserInfo::MsgUserInfo(const MsgUserInfo& other)
		: Msg(other), m_info(bufferAs<MsgInfo>())
	{

	}

	MsgUserInfo& MsgUserInfo::operator=(MsgUserInfo&& other) noexcept
	{
		Msg::operator=(std::move(other));
		m_info = bufferAs<MsgInfo>();

		return *this;
	}

	MsgUserInfo& MsgUserInfo::operator=(const MsgUserInfo& other)
	{
		Msg::operator=(other);
		m_info = bufferAs<MsgInfo>();

		return *this;
	}

	void MsgUserInfo::create(const Player& aPlayer)
	{
		assert(aPlayer.name().size() < MAX_NAMESIZE);
		assert(aPlayer.mate().size() < MAX_NAMESIZE);

		m_info->Header.Length = static_cast<uint16_t>(length());
		m_info->Header.Type = MSG_USERINFO;

		m_info->UniqId = aPlayer.uid();
		m_info->Look = aPlayer.look();
		m_info->Hair = aPlayer.hair();
		m_info->Money = aPlayer.money();
		m_info->Exp = aPlayer.experience();
		m_info->Force = aPlayer.force();
		m_info->Health = aPlayer.health();
		m_info->Dexterity = aPlayer.dexterity();
		m_info->Soul = aPlayer.soul();
		m_info->AddPoints = aPlayer.addPoints();
		m_info->CurHP = aPlayer.curHP();
		m_info->CurMP = aPlayer.curMP();
		m_info->PkPoints = aPlayer.pkPoints();
		m_info->Level = aPlayer.level();
		m_info->Profession = aPlayer.profession();
		m_info->AutoAllot = aPlayer.autoAllot() ? 1 : 0;
		m_info->Metempsychosis = aPlayer.metempsychosis();
		m_info->ShowName = 1;

		StringPacker packer(m_info->StringPack);
		packer.addString(aPlayer.name());
		packer.addString(aPlayer.mate());
	}
}