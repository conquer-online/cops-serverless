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

#ifndef ZFSERVER_NETWORK_MSG_CONNECT_H
#define ZFSERVER_NETWORK_MSG_CONNECT_H

#include "msg.h"

namespace zfserver::network
{
    /**
     * Msg sent by the AccServer to answer to a connection request.
     */
    class MsgConnect final : public Msg
    {
    public:
        #pragma pack(push, 1)
        typedef struct
        {
            /** Generic header of all msgs */
            Msg::Header Header;
            /** THe account UID (key B) */
            int32_t AccountUID;
            /** The token / session ID of the connection (key A) */
            int32_t Data;
            /** The information (IP address) of the game server */
            char Info[MAX_NAMESIZE];
        }MsgInfo;
        #pragma pack(pop)

    public:
        /**
         * Create a message object from the specified buffer.
         *
         * @param[in] buf  the buffer to copy
         * @param[in] len  the length in bytes of the buffer
         */
        MsgConnect(const uint8_t* buf, size_t len);

        MsgConnect(MsgConnect&& other) noexcept;
        MsgConnect(const MsgConnect& other);
        MsgConnect& operator=(MsgConnect&& other) noexcept;
        MsgConnect& operator=(const MsgConnect& other);

        /* destructor */
        ~MsgConnect() = default;

        /**
         * Process the message received from the client.
         *
         * @param[in] client      the client which has sent the message
         * @param[in] connection  the connection on which the message was sent
         */
        void process(Client& client, Connection& connection) override;

    private:
        MsgInfo* m_info; //!< the casted internal reference to the buffer
    };
}

#endif // ZFSERVER_NETWORK_MSG_CONNECT_H
