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

#ifndef ZFSERVER_NETWORK_MSG_CONNECTEX_H
#define ZFSERVER_NETWORK_MSG_CONNECTEX_H

#include "msg.h"

#include <string_view>

namespace zfserver::network
{
	/**
	 * Msg sent by the AccServer to answer to a connection request.
	 */
	class MsgConnectEx final : public Msg
	{
	public:
#pragma pack(push, 1)
		typedef struct
		{
			/** Generic header of all msgs */
			Msg::Header Header;
			/** THe account UID */
			int32_t AccountUID;
			/** The token / session ID of the connection  */
			int32_t Data;
			/** The information (IP address) of the MsgServer or the error string */
			char Info[MAX_NAMESIZE];
			/** The port of the MsgServer. */
			uint32_t Port;
		}MsgInfo;
#pragma pack(pop)

	public:
		/**
		 * Create a new MsgConnect for the specified account.
		 *
		 * @param aAccUID[in]   the account UID
		 * @param aData[in]     the session ID
		 * @param aInfo[in]     the game server IP address
		 * @param aPort[in]     the game server port
		 */
		MsgConnectEx(int32_t aAccUID, int32_t aData, std::string_view aInfo, uint16_t aPort);

		MsgConnectEx(MsgConnectEx&& other) noexcept;
		MsgConnectEx(const MsgConnectEx& other);
		MsgConnectEx& operator=(MsgConnectEx&& other) noexcept;
		MsgConnectEx& operator=(const MsgConnectEx& other);

		/* destructor */
		~MsgConnectEx() = default;

	private:
		/* internal filling of the packet */
		void create(int32_t aAccUID, int32_t aData, std::string_view aInfo, uint16_t aPort);

	private:
		MsgInfo* m_info; //!< the casted internal reference to the buffer
	};
}

#endif // ZFSERVER_NETWORK_MSG_CONNECTEX_H
