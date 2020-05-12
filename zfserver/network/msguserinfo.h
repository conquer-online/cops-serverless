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

#ifndef ZFSERVER_NETWORK_MSG_USERINFO_H
#define ZFSERVER_NETWORK_MSG_USERINFO_H

#include "msg.h"

namespace zfserver
{
    class Player;
}

namespace zfserver::network
{
	/**
	 * Msg sent to the client by the MsgServer to fill all the player variables.
	 */
	class MsgUserInfo final : public Msg
	{
	public:
#pragma pack(push, 1)
		typedef struct
		{
            /** Generic header of all msgs */
            Msg::Header Header;
            /** The unique Id of the player */
            uint32_t UniqId;
            /** The look of the player */
            uint32_t Look;
            /** The hair of the player */
            uint16_t Hair;
            /** The length of the player (unused by Conquer Online) */
            uint8_t Length;
            /** The fat of the player (unused by Conquer Online) */
            uint8_t Fat;
            /** The money of the player */
            uint32_t Money;
            /** The experience of the player */
            uint32_t Exp;
            /** Unknown bytes */
            uint8_t Padding[16];
            /** The force of the player */
            uint16_t Force;
            /** The dexterity of the player */
            uint16_t Dexterity;
            /** The health of the player */
            uint16_t Health;
            /** The soul of the player */
            uint16_t Soul;
            /** The additional points of the player */
            uint16_t AddPoints;
            /** The hit points of the player */
            uint16_t CurHP;
            /** The mana points of the player */
            uint16_t CurMP;
            /** The pk points of the player */
            int16_t PkPoints;
            /** The level of the player */
            uint8_t Level;
            /** The profession of the player */
            uint8_t Profession;
            /** If the server handle the allot points */
            uint8_t AutoAllot; // bool
            /** The metempsychosis of the player */
            uint8_t Metempsychosis;
            /** If the client must show the name of the player */
            uint8_t ShowName; // bool
            /** Name / Mate of the player */
            uint8_t StringPack[1];
		}MsgInfo;
#pragma pack(pop)

	public:
        /**
         * Create a new MsgUserInfo packet for the specified player.
         *
         * @param[in]   aPlayer     a reference to the player object
         */
        MsgUserInfo(const Player& aPlayer);

        MsgUserInfo(MsgUserInfo&& other) noexcept;
        MsgUserInfo(const MsgUserInfo& other);
        MsgUserInfo& operator=(MsgUserInfo&& other) noexcept;
        MsgUserInfo& operator=(const MsgUserInfo& other);

		/* destructor */
		~MsgUserInfo() = default;

	private:
        /* internal filling of the packet */
        void create(const Player& aPlayer);

	private:
		MsgInfo* m_info; //!< the casted internal reference to the buffer
	};
}

#endif // ZFSERVER_NETWORK_MSG_USERINFO_H
