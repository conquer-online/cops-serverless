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

#ifndef ZFSERVER_NETWORK_DEFINITIONS_H
#define ZFSERVER_NETWORK_DEFINITIONS_H

#include <cstddef>
#include <cstdint>

namespace zfserver::network
{
    /** Maximum length of a name, including the NUL character. */
    constexpr size_t MAX_NAMESIZE = 16;
    /** Maximum length of a message, including the NUL character. */
    constexpr size_t MAX_WORDSSIZE = 255;
    /** Maximum length of a parameter, including the NUL character. */
    constexpr size_t MAX_PARAMSIZE = 255;

    constexpr uint16_t MSG_NONE = 0;
    constexpr uint16_t MSG_GENERAL = 1000;

    constexpr uint16_t MSG_REGISTER         = MSG_GENERAL + 1;
    constexpr uint16_t MSG_LOGIN            = MSG_GENERAL + 2;
    constexpr uint16_t MSG_LOGOUT           = MSG_GENERAL + 3;
    constexpr uint16_t MSG_TALK             = MSG_GENERAL + 4;
    constexpr uint16_t MSG_WALK             = MSG_GENERAL + 5;
    constexpr uint16_t MSG_USERINFO         = MSG_GENERAL + 6;
    constexpr uint16_t MSG_ATTACK           = MSG_GENERAL + 7;
    constexpr uint16_t MSG_ITEm_info         = MSG_GENERAL + 8;
    constexpr uint16_t MSG_ITEM             = MSG_GENERAL + 9;
    constexpr uint16_t MSG_ACTION           = MSG_GENERAL + 10;
    constexpr uint16_t MSG_ACCIDENT         = MSG_GENERAL + 11;
    constexpr uint16_t MSG_TICK             = MSG_GENERAL + 12;
    constexpr uint16_t MSG_ROOM             = MSG_GENERAL + 13;
    constexpr uint16_t MSG_PLAYER           = MSG_GENERAL + 14;
    constexpr uint16_t MSG_NAME             = MSG_GENERAL + 15;
    constexpr uint16_t MSG_WEATHER          = MSG_GENERAL + 16;
    constexpr uint16_t MSG_USERATTRIB       = MSG_GENERAL + 17;
    constexpr uint16_t MSG_ROLE             = MSG_GENERAL + 18;
    constexpr uint16_t MSG_FRIEND           = MSG_GENERAL + 19;
    constexpr uint16_t MSG_EFFECT           = MSG_GENERAL + 20;
    constexpr uint16_t MSG_QUIZGAME         = MSG_GENERAL + 21;
    constexpr uint16_t MSG_INTERACT         = MSG_GENERAL + 22;
    constexpr uint16_t MSG_TEAM             = MSG_GENERAL + 23;
    constexpr uint16_t MSG_ALLOT            = MSG_GENERAL + 24;
    constexpr uint16_t MSG_WEAPONSKILL      = MSG_GENERAL + 25;
    constexpr uint16_t MSG_TEAMMEMBER       = MSG_GENERAL + 26;
    constexpr uint16_t MSG_GEMEMBED         = MSG_GENERAL + 27;
    constexpr uint16_t MSG_DATE             = MSG_GENERAL + 33;

    constexpr uint16_t MSG_ACCOUNT          = MSG_GENERAL + 51;
    constexpr uint16_t MSG_CONNECT          = MSG_GENERAL + 52;
    constexpr uint16_t MSG_CONNECTEX        = MSG_GENERAL + 55;
    constexpr uint16_t MSG_TRADE            = MSG_GENERAL + 56;
    constexpr uint16_t MSG_BATTLESYSTEM     = MSG_GENERAL + 57;

    constexpr uint16_t MSG_MAPITEM          = MSG_GENERAL + 101;
    constexpr uint16_t MSG_PACKAGE          = MSG_GENERAL + 102;
    constexpr uint16_t MSG_MAGICINFO        = MSG_GENERAL + 103;
    constexpr uint16_t MSG_FLUSHEXP         = MSG_GENERAL + 104;
    constexpr uint16_t MSG_MAGICEFFECT      = MSG_GENERAL + 105;
    constexpr uint16_t MSG_SYNATTRINFO      = MSG_GENERAL + 106;
    constexpr uint16_t MSG_SYNDICATE        = MSG_GENERAL + 107;
    constexpr uint16_t MSG_ITEm_infoEX       = MSG_GENERAL + 108;
    constexpr uint16_t MSG_NPCINFOEX        = MSG_GENERAL + 109;
    constexpr uint16_t MSG_MAPINFO          = MSG_GENERAL + 110;
    constexpr uint16_t MSG_MESSAGEBOARD     = MSG_GENERAL + 111;
    constexpr uint16_t MSG_SYNMEMBERINFO    = MSG_GENERAL + 112;
    constexpr uint16_t MSG_DICE             = MSG_GENERAL + 113;
    constexpr uint16_t MSG_SYNINFO          = MSG_GENERAL + 114;

    constexpr uint16_t MSG_NPCINFO          = MSG_GENERAL + 1030;
    constexpr uint16_t MSG_NPC              = MSG_GENERAL + 1031;
    constexpr uint16_t MSG_DIALOG           = MSG_GENERAL + 1032;
    constexpr uint16_t MSG_FRIENDINFO       = MSG_GENERAL + 1033;
    constexpr uint16_t MSG_TIME             = MSG_GENERAL + 1034;
    constexpr uint16_t MSG_PETINFO          = MSG_GENERAL + 1035;
    constexpr uint16_t MSG_DATAARRAY        = MSG_GENERAL + 1036;
    constexpr uint16_t MSG_EUDEMONATTRIB    = MSG_GENERAL + 1037;
    constexpr uint16_t MSG_SCHOOLMEMBER     = MSG_GENERAL + 1038;
    constexpr uint16_t MSG_PLAYERTASK       = MSG_GENERAL + 1039;
    constexpr uint16_t MSG_TASK_LIST        = MSG_GENERAL + 1040;
    constexpr uint16_t MSG_ANNOUNCE_LIST    = MSG_GENERAL + 1041;
    constexpr uint16_t MSG_ANNOUNCE_INFO    = MSG_GENERAL + 1042;
    constexpr uint16_t MSG_AUCTION          = MSG_GENERAL + 1043;
    constexpr uint16_t MSG_CHATROOM         = MSG_GENERAL + 1044;
    constexpr uint16_t MSG_ITEMATTRIB       = MSG_GENERAL + 1045;
}

#endif // ZFSERVER_NETWORK_DEFINITIONS_H
