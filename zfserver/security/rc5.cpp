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

#include "rc5.h"

#include <cassert>
#include <cstring>

#include <algorithm>
#include <type_traits>

namespace zfserver::security
{
	namespace
	{
		inline uint32_t rotl(uint32_t value, uint32_t count)
		{
			count %= 32;
			return (value >> (32 - count)) | (value << count);
		}

		inline uint32_t rotr(uint32_t value, uint32_t count)
		{
			count %= 32;
			return (value >> count) | (value << (32 - count));
		}
	}

	RC5::RC5(const uint8_t seed[KEY_SIZE]) noexcept
	{
		generateKey(seed);
	}

	void RC5::generateKey(const uint8_t seed[KEY_SIZE]) noexcept
	{
		assert(seed != nullptr);

		static constexpr uint32_t PW32 = std::integral_constant<uint32_t, 0xB7E15163>::value;
		static constexpr uint32_t QW32 = std::integral_constant<uint32_t, 0x9E3779B9>::value;

		uint32_t key[KEY32_SIZE];
		std::memcpy(key, seed, KEY_SIZE);

		size_t i, j;
		uint32_t x, y;

		m_sub[0] = PW32;
		for (i = 1; i < SUB32_SIZE; ++i)
			m_sub[i] = m_sub[i - 1] + QW32;

		i = j = 0;
		x = y = 0;
		for (size_t k = 0, size = 3 * std::max(KEY32_SIZE, SUB32_SIZE); k < size; ++k)
		{
			m_sub[i] = rotl((m_sub[i] + x + y), 3);
			x = m_sub[i];
			i = (i + 1) % SUB32_SIZE;
			key[j] = rotl((key[j] + x + y), (x + y));
			y = key[j];
			j = (j + 1) % KEY32_SIZE;
		}
	}

	void RC5::encrypt(uint8_t* buf, size_t len) noexcept
	{
		assert(buf != nullptr);
		assert(len % BLOCK_SIZE == 0);

		uint32_t* block = reinterpret_cast<uint32_t*>(buf);
		for (size_t i = 0, bc = len / BLOCK_SIZE; i != bc; ++i)
		{
			uint32_t a = block[2 * i];
			uint32_t b = block[2 * i + 1];

			uint32_t le = a + m_sub[0];
			uint32_t re = b + m_sub[1];
			for (size_t j = 1; j <= ROUNDS; ++j)
			{
				le = rotl((le ^ re), re) + m_sub[2 * j];
				re = rotl((re ^ le), le) + m_sub[2 * j + 1];
			}

			block[2 * i] = le;
			block[2 * i + 1] = re;
		}
	}

	void RC5::decrypt(uint8_t* buf, size_t len) noexcept
	{
		assert(buf != nullptr);
		assert(len % BLOCK_SIZE == 0);

		uint32_t* block = reinterpret_cast<uint32_t*>(buf);
		for (size_t i = 0, bc = len / BLOCK_SIZE; i != bc; ++i)
		{
			uint32_t ld = block[2 * i];
			uint32_t rd = block[2 * i + 1];

			for (size_t j = ROUNDS; j >= 1; --j)
			{
				rd = rotr((rd - m_sub[2 * j + 1]), ld) ^ ld;
				ld = rotr((ld - m_sub[2 * j]), rd) ^ rd;
			}
			uint32_t a = ld - m_sub[0];
			uint32_t b = rd - m_sub[1];

			block[2 * i] = a;
			block[2 * i + 1] = b;
		}
	}
}
