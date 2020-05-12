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

#ifndef ZFSERVER_NETWORK_MSG_TALK_H
#define ZFSERVER_NETWORK_MSG_TALK_H

#include "msg.h"

#include <string_view>

namespace zfserver::network
{
	enum class Channel : uint16_t
	{
		Normal = 2000,
		Private = Normal + 1,
		Action = Normal + 2,
		Team = Normal + 3,
		Syndicate = Normal + 4,
		System = Normal + 5,
		Family = Normal + 6,
		Talk = Normal + 7,
		Yelp = Normal + 8,
		Friend = Normal + 9,
		Global = Normal + 10,
		GM = Normal + 11,
		Whisper = Normal + 12,
		Ghost = Normal + 13,
		Serve = Normal + 14,
		My = Normal + 15,

		Register = Normal + 100,
		Entrance = Normal + 101,
		Shop = Normal + 102,
		PetTalk = Normal + 103,
		CryOut = Normal + 104,
		WebPage = Normal + 105,

		NewMessage = Normal + 106,
		Task = Normal + 107,

		SynWar_First = Normal + 108,
		SynWar_Next = Normal + 109,

		LeaveWord = Normal + 110,
		CHANNEL_SYNANNOUNCEG = Normal + 111,
		CHANNEL_SYNANNOUNCEL = Normal + 114,

		MessageBox = Normal + 112,
		Reject = Normal + 113,

		Trace = Normal + 115,

		BoardTrade = Normal + 201,
		BoardFriend = Normal + 202,
		BoardTeam = Normal + 203,
		BoardSyndicate = Normal + 204,
		BoardOther = Normal + 205
	};

	enum class Color : uint32_t
	{
		Red = 0x00FF0000,
		Magenta = 0x00FF00FF,
		Blue = 0x000000FF,
		Cyan = 0x0000FFFF,
		Green = 0x0000FF00,
		Yellow = 0x00FFFF00,
		White = 0x00FFFFFF,
	};

	/**
	 * Msg sent by the AccServer to answer to a connection request.
	 */
	class MsgTalk final : public Msg
	{
	public:
		enum class Style : uint16_t
		{
			Normal = 0b0000000000000000,
			Scroll = 0b0000000000000001,
			Flash = 0b0000000000000010,
			Blast = 0b0000000000000100,
		};

	public:
#pragma pack(push, 1)
		typedef struct
		{
			/** Generic header of all msgs */
			Msg::Header Header;
			Color Color; // ARGB code
			Channel Channel;
			Style Style;
			int32_t Timestamp;
			uint8_t StringPack[1]; // Speaker, Hearer, Emotion, Words
		}MsgInfo;
#pragma pack(pop)

	public:
		MsgTalk(std::string_view speaker, std::string_view hearer, std::string_view words,
			Channel channel, Color color = Color::White);

		/**
		 * Create a message object from the specified buffer.
		 *
		 * @param[in] buf  the buffer to copy
		 * @param[in] len  the length in bytes of the buffer
		 */
		MsgTalk(const uint8_t* buf, size_t len);

		MsgTalk(MsgTalk&& other) noexcept;
		MsgTalk(const MsgTalk& other);
		MsgTalk& operator=(MsgTalk&& other) noexcept;
		MsgTalk& operator=(const MsgTalk& other);

		/* destructor */
		~MsgTalk() override = default;

		/**
		 * Process the message received from the client.
		 *
		 * @param[in] client      the client which has sent the message
		 * @param[in] connection  the connection on which the message was sent
		 */
		void process(Client& client, Connection& connection) override;

	private:
		/* internal filling of the packet */
		void create(std::string_view speaker, std::string_view hearer, std::string_view emotion, std::string_view words,
			Channel channel, Color color);

	private:
		MsgInfo* m_info; //!< the casted internal reference to the buffer
	};
}

#endif // ZFSERVER_NETWORK_MSG_TALK_H
