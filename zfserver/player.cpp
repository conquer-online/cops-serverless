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

#include "player.h"

namespace zfserver
{
	uint32_t Player::uid() const noexcept
	{
		return m_uid;
	}

	const std::string& Player::name() const noexcept
	{
		return m_name;
	}

	const std::string& Player::mate() const noexcept
	{
		return m_mate;
	}

	uint32_t Player::look() const noexcept
	{
		return m_look;
	}

	uint16_t Player::hair() const noexcept
	{
		return m_hair;
	}

	uint32_t Player::money() const noexcept
	{
		return m_money;
	}

	uint8_t Player::level() const noexcept
	{
		return m_level;
	}

	uint32_t Player::experience() const noexcept
	{
		return m_experience;
	}

	uint8_t Player::profession() const noexcept
	{
		return m_profession;
	}

	uint8_t Player::metempsychosis() const noexcept
	{
		return m_metempsychosis;
	}

	bool Player::autoAllot() const noexcept
	{
		return level() <= 120 && metempsychosis() == 0;
	}

	uint16_t Player::force() const noexcept
	{
		return m_force;
	}

	uint16_t Player::dexterity() const noexcept
	{
		return m_dexterity;
	}

	uint16_t Player::health() const noexcept
	{
		return m_health;
	}

	uint16_t Player::soul() const noexcept
	{
		return m_soul;
	}

	uint16_t Player::addPoints() const noexcept
	{
		return m_addPoints;
	}

	uint16_t Player::curHP() const noexcept
	{
		return m_curHP;
	}

	uint16_t Player::curMP() const noexcept
	{
		return m_curMP;
	}

	int16_t Player::pkPoints() const noexcept
	{
		return m_pkPoints;
	}
}