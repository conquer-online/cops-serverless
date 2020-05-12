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

#include "stringpacker.h"

#include <cassert>
#include <cstring>
#include <limits>

namespace zfserver::network
{
	StringPacker::StringPacker(uint8_t* buf)
		: m_buffer(buf), m_count(*buf)
	{
		assert(buf != nullptr);
	}

	void StringPacker::addString(std::string_view str)
	{
		assert(str.size() <= std::numeric_limits<uint8_t>::max());

		char* ptr = reinterpret_cast<char*>(m_buffer + 1);
		for (uint8_t i = 0; i < m_count; ++i)
			ptr += *reinterpret_cast<uint8_t*>(ptr++); // the length...

		*reinterpret_cast<uint8_t*>(ptr++) = static_cast<uint8_t>(str.size());
		std::memcpy(ptr, str.data(), str.size());

		*m_buffer = ++m_count;
	}

	std::optional<std::string_view> StringPacker::getString(const uint8_t index) const
	{
		std::optional<std::string_view> str;

		if (index < m_count)
		{
			char* ptr = reinterpret_cast<char*>(m_buffer + 1);
			for (uint8_t i = 0; i < index; ++i)
				ptr += *reinterpret_cast<uint8_t*>(ptr++); // the length...

			uint8_t len = *reinterpret_cast<uint8_t*>(ptr++);
			str = std::string_view{ ptr, len };
		}

		return str;
	}
}