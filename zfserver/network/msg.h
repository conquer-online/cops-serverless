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

#ifndef ZFSERVER_NETWORK_MSG_H
#define ZFSERVER_NETWORK_MSG_H

#include <cstddef>
#include <cstdint>

#include <memory>

#include "networkdef.h"

namespace zfserver
{
	class Client;
	class Connection;
}

namespace zfserver::network
{
	/**
	 * The base class for all networking messages.
	 */
	class Msg
	{
	public:
		/**
		 * The general header prefixing all messages.
		 */
		typedef struct
		{
			/** The length of the message in bytes */
			uint16_t Length;
			/** The type of the message */
			uint16_t Type;
		}Header;

	public:
		/**
		 * Create a message object from the specified buffer.
		 * The buffer will be took by the object and the memory
		 * freed when the object will be destroyed.

		 * @param[in,out] buf  a pointer to the buffer to take
							   the pointer will be set to null
		 * @param[in]     len  the length in bytes of the buffer
		 */
		static [[nodiscard]] std::unique_ptr<Msg> create(const uint8_t* buf, size_t len);

		/**
		 * Print the msg in the standard output stream.
		 */
		static void dump(const uint8_t* buf, size_t len);

		/**
		 * Print the msg in the standard output stream.
		 */
		static void dump(Msg& msg);

	public:
		/**
		 * Create a message object from the specified buffer.
		 *
		 * @param[in] buf  the buffer to copy
		 * @param[in] len  the length in bytes of the buffer
		 */
		Msg(const uint8_t* buf, size_t len);

		Msg(Msg&& other) noexcept;
		Msg(const Msg& other);
		Msg& operator=(Msg&& other) noexcept;
		Msg& operator=(const Msg& other);

		/**
		 * Delete the message and the buffer.
		 */
		virtual ~Msg() = default;

		/**
		 * Process the message received from the client.
		 *
		 * @param[in] client      the client which has sent the message
		 * @param[in] connection  the connection on which the message was sent
		 */
		virtual void process(Client& client, Connection& connection);

	public:
		/** Get a pointer of the buffer. It may not be the internal one. */
		[[nodiscard]] const uint8_t* buffer() const noexcept { return m_buffer.get(); }

		/** Get the length in bytes of the message. */
		[[nodiscard]] size_t length() const noexcept { return m_length; }

	protected:
		/**
		 * Create a message object with an internal buffer of
		 * the specified length.
		 *
		 * @param[in] len  the length in bytes of the buffer
		 */
		Msg(size_t len);

		/** Reinterpret the buffer as T. */
		template<typename T>
		[[nodiscard]] T* bufferAs() noexcept { return reinterpret_cast<T*>(m_buffer.get()); }

	private:
		std::unique_ptr<uint8_t[]> m_buffer; //!< the internal buffer
		size_t m_length; //!< the length in bytes of the buffer
	};
}

#endif // ZFSERVER_NETWORK_MSG_H
