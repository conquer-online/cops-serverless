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

#ifndef ZFSERVER_PLAYER_H
#define ZFSERVER_PLAYER_H

#include <cstdint>
#include <string>

namespace zfserver
{
	class Player final
	{
	public:
		Player() = default;
		~Player() = default;

		Player(Player&& other) = delete;
		Player(const Player& other) = delete;
		Player& operator=(Player&& other) = delete;
		Player& operator=(const Player& other) = delete;

		uint32_t uid() const noexcept;
		const std::string& name() const noexcept;
		const std::string& mate() const noexcept;
		uint32_t look() const noexcept;
		uint16_t hair() const noexcept;
		uint32_t money() const noexcept;

		uint8_t level() const noexcept;
		uint32_t experience() const noexcept;
		uint8_t profession() const noexcept;
		uint8_t metempsychosis() const noexcept;
		bool autoAllot() const noexcept;

		uint16_t force() const noexcept;
		uint16_t dexterity() const noexcept;
		uint16_t health() const noexcept;
		uint16_t soul() const noexcept;
		uint16_t addPoints() const noexcept;

		uint16_t curHP() const noexcept;
		uint16_t curMP() const noexcept;

		int16_t pkPoints() const noexcept;

	private:
		uint32_t m_uid = 1000001;
		std::string m_name = "CptSky[PM]";
		std::string m_mate = "None";
		uint32_t m_look = 67'1003;
		uint16_t m_hair = 311;
		uint32_t m_money = 2'000'000;
		uint8_t m_level = 125;
		uint32_t m_experience = 1234872;
		uint8_t m_profession = 15;
		uint8_t m_metempsychosis = 2;
		uint16_t m_force = 185;
		uint16_t m_dexterity = 72;
		uint16_t m_health = 110;
		uint16_t m_soul = 12;
		uint16_t m_addPoints = 3;
		uint16_t m_curHP = 1250;
		uint16_t m_curMP = 30;
		int16_t m_pkPoints = 30000;
	};
}

#endif // ZFSERVER_PLAYER_H