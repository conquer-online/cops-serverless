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

#ifndef ZFSERVER_NETWORK_STRINGPACKER_H
#define ZFSERVER_NETWORK_STRINGPACKER_H

#include <cstddef>
#include <cstdint>

#include <optional>
#include <string_view>

namespace zfserver::network
{
	/**
	 * String packer for an internal buffer. All strings are prefixed with
	 * their length in an unsigned 8-bit integer.
	 *
	 * When extracting string, the first string is at the index 0.
	 */
	class StringPacker final
	{
	public:
		/**
		 * Create a new packer around the specified buffer.
		 *
		 * @param buf[in]  the buffer of the string pack
		 */
		StringPacker(uint8_t* buf);

		/* destructor */
		~StringPacker() = default;

		StringPacker(StringPacker&&) = delete;
		StringPacker(const StringPacker&) = delete;
		StringPacker& operator=(StringPacker&&) = delete;
		StringPacker& operator=(const StringPacker&) = delete;

		/**
		 * Append a string at the end of the pack.
		 *
		 * @param str[in]  the string to append at the end
		 */
		void addString(std::string_view str);

		/**
		 * Extract a string from the pack.
		 *
		 * @param index[in]  the index of the string to retreive (0 is the first string)
		 */
		std::optional<std::string_view> getString(uint8_t index) const;

	private:
		uint8_t* m_buffer; //!< reference to the internal buffer
		uint8_t m_count; //!< the number of strings in the pack
	};
}

#endif // ZFSERVER_NETWORK_STRINGPACKER_H
