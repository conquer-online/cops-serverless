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

#include "tqcipher.h"

#include <cassert>
#include <cstring>

#include <type_traits>

#if defined(__AVX2__)
#   define _vector_load(p)         _mm256_load_si256(p)
#   define _vector_loadu(p)        _mm256_loadu_si256(p)
#   define _vector_storeu(p, b)    _mm256_storeu_si256(p, b)
#   define _vector_set1_epi8(val)  _mm256_set1_epi8(val)
#   define _vector_xor(a, b)       _mm256_xor_si256(a, b)
#   define _vector_or(a, b)        _mm256_or_si256(a, b)
#   define _vector_slli_epi8(a, c) _mm256_slli_epi8(a, c)
#   define _vector_srli_epi8(a, c) _mm256_srli_epi8(a, c)
#elif defined(__SSE2__) || (defined(_M_IX86_FP) && _M_IX86_FP == 2)
#   define _vector_load(p)         _mm_load_si128(p)
#   define _vector_loadu(p)        _mm_loadu_si128(p)
#   define _vector_storeu(p, b)    _mm_storeu_si128(p, b)
#   define _vector_set1_epi8(val)  _mm_set1_epi8(val)
#   define _vector_xor(a, b)       _mm_xor_si128(a, b)
#   define _vector_or(a, b)        _mm_or_si128(a, b)
#   define _vector_slli_epi8(a, c) _mm_slli_epi8(a, c)
#   define _vector_srli_epi8(a, c) _mm_srli_epi8(a, c)
#endif

namespace zfserver::security
{
	namespace
	{
#if defined(__AVX2__)
		__forceinline __m256i _mm256_slli_epi8(__m256i __a, int __count)
		{
			static constexpr uint8_t MASKS[] = { 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80 };
			return _mm256_and_si256(_mm256_slli_epi16(__a, __count), _mm256_set1_epi8(MASKS[__count]));
		}

		__forceinline __m256i _mm256_srli_epi8(__m256i __a, int __count)
		{
			static constexpr uint8_t MASKS[] = { 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01 };
			return _mm256_and_si256(_mm256_srli_epi16(__a, __count), _mm256_set1_epi8(MASKS[__count]));
		}
#elif defined(__SSE2__) || (defined(_M_IX86_FP) && _M_IX86_FP == 2)
		__forceinline __m128i _mm_slli_epi8(__m128i __a, int __count)
		{
			static constexpr uint8_t MASKS[] = { 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0x80 };
			return _mm_and_si128(_mm_slli_epi16(__a, __count), _mm_set1_epi8(MASKS[__count]));
		}

		__forceinline __m128i _mm_srli_epi8(__m128i __a, int __count)
		{
			static constexpr uint8_t MASKS[] = { 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01 };
			return _mm_and_si128(_mm_srli_epi16(__a, __count), _mm_set1_epi8(MASKS[__count]));
		}
#endif
	}

	TqCipher::TqCipher() noexcept
	{
		static constexpr uint32_t P = std::integral_constant<uint32_t, 0x13FA0F9D>::value;
		static constexpr uint32_t G = std::integral_constant<uint32_t, 0x6D5C7962>::value;

		const uint8_t* p = reinterpret_cast<const uint8_t*>(&P);
		const uint8_t* g = reinterpret_cast<const uint8_t*>(&G);

		uint8_t* key1 = m_key;
		uint8_t* key2 = key1 + KEY_OFFSET;

		uint8_t p0 = p[0];
		uint8_t g0 = g[0];
		for (size_t i = 0; i != PARTIAL_KEY_SIZE; ++i)
		{
			key1[i] = p0;
			key2[i] = g0;
			p0 = ((p[1] + static_cast<uint8_t>(p0 * p[2])) * p0 + p[3]);
			g0 = ((g[1] - static_cast<uint8_t>(g0 * g[2])) * g0 + g[3]);
		}

#if defined(__SSE2__) || defined(__AVX2__) || (defined(_M_IX86_FP) && _M_IX86_FP == 2)
		std::memcpy(key1 + PARTIAL_KEY_SIZE, key1, KEY_PADDING);
		std::memcpy(key2 + PARTIAL_KEY_SIZE, key2, KEY_PADDING);
#endif
	}

	void TqCipher::generateAltKey(int32_t a, int32_t b) noexcept
	{
		const uint32_t x = static_cast<uint32_t>(((a + b) ^ 0x4321) ^ a);
		const uint32_t y = x * x;
		const uint8_t* tmpKey1 = reinterpret_cast<const uint8_t*>(&x);
		const uint8_t* tmpKey2 = reinterpret_cast<const uint8_t*>(&y);

		const uint8_t* key1 = m_key;
		const uint8_t* key2 = key1 + KEY_OFFSET;
		uint8_t* altKey1 = m_altKey;
		uint8_t* altKey2 = altKey1 + KEY_OFFSET;

		for (size_t i = 0; i != PARTIAL_KEY_SIZE; ++i)
		{
			altKey1[i] = key1[i] ^ tmpKey1[(i % sizeof(x))];
			altKey2[i] = key2[i] ^ tmpKey2[(i % sizeof(y))];
		}

#if defined(__SSE2__) || defined(__AVX2__) || (defined(_M_IX86_FP) && _M_IX86_FP == 2)
		std::memcpy(altKey1 + PARTIAL_KEY_SIZE, altKey1, KEY_PADDING);
		std::memcpy(altKey2 + PARTIAL_KEY_SIZE, altKey2, KEY_PADDING);
#endif

		m_usingAltKey = true;
		m_encryptCounter = 0;
	}

	void TqCipher::encrypt(uint8_t* buf, size_t len) noexcept
	{
		assert(buf != nullptr);

		const uint8_t* key1 = m_key;
		const uint8_t* key2 = key1 + KEY_OFFSET;

		size_t i = 0;

#if defined(__SSE2__) || defined(__AVX2__) || (defined(_M_IX86_FP) && _M_IX86_FP == 2)
		alignas(vector_t) uint8_t tmp[sizeof(vector_t)];
		vector_t* block = reinterpret_cast<vector_t*>(buf);
		vector_t x, y, z, w;

		z = _vector_set1_epi8(0xABU);
		for (size_t i = 0, count = len / sizeof(vector_t); i != count; ++i)
		{
			x = _vector_loadu(reinterpret_cast<const vector_t*>(&key1[(m_encryptCounter) & 0xFF]));

			const size_t n = 0x100 - m_encryptCounter % 0x100;
			if (n >= sizeof(vector_t))
			{
				y = _vector_set1_epi8(key2[(m_encryptCounter >> 8) & 0xFF]);
			}
			else
			{
				std::memset(tmp, key2[(m_encryptCounter >> 8) & 0xFF], n);
				std::memset(&tmp[n], key2[(uint8_t)(m_encryptCounter >> 8) + 1], sizeof(vector_t) - n);
				y = _vector_load(reinterpret_cast<vector_t*>(tmp));
			}

			w = _vector_loadu(&block[i]);

			w = _vector_xor(w, z);
			w = _vector_or(_vector_slli_epi8(w, 4), _vector_srli_epi8(w, 4));
			w = _vector_xor(_vector_xor(w, x), y);

			_vector_storeu(&block[i], w);

			m_encryptCounter += sizeof(vector_t);
		}

		i = len - (len % sizeof(vector_t));
#endif

		for (; i != len; ++i)
		{
			buf[i] ^= 0xAB;
			buf[i] = static_cast<uint8_t>(buf[i] << 4 | buf[i] >> 4);
			buf[i] ^= key1[(m_encryptCounter) & 0xFF];
			buf[i] ^= key2[(m_encryptCounter >> 8) & 0xFF];
			++m_encryptCounter;
		}
	}

	void TqCipher::decrypt(uint8_t* buf, size_t len) noexcept
	{
		assert(buf != nullptr);

		const uint8_t* key1 = m_usingAltKey ? m_altKey : m_key;
		const uint8_t* key2 = key1 + KEY_OFFSET;

		size_t i = 0;

#if defined(__SSE2__) || defined(__AVX2__) || (defined(_M_IX86_FP) && _M_IX86_FP == 2)
		alignas(vector_t) uint8_t tmp[sizeof(vector_t)];
		vector_t* block = reinterpret_cast<vector_t*>(buf);
		vector_t x, y, z, w;

		z = _vector_set1_epi8(0xABU);
		for (size_t i = 0, count = len / sizeof(vector_t); i != count; ++i)
		{
			x = _vector_loadu(reinterpret_cast<const vector_t*>(&key1[(m_decryptCounter) & 0xFF]));

			const size_t n = 0x100 - m_decryptCounter % 0x100;
			if (n >= sizeof(vector_t))
			{
				y = _vector_set1_epi8(key2[(m_decryptCounter >> 8) & 0xFF]);
			}
			else
			{
				std::memset(tmp, key2[(m_decryptCounter >> 8) & 0xFF], n);
				std::memset(&tmp[n], key2[(uint8_t)(m_decryptCounter >> 8) + 1], sizeof(vector_t) - n);
				y = _vector_load(reinterpret_cast<vector_t*>(tmp));
			}

			w = _vector_loadu(&block[i]);

			w = _vector_xor(w, z);
			w = _vector_or(_vector_slli_epi8(w, 4), _vector_srli_epi8(w, 4));
			w = _vector_xor(_vector_xor(w, x), y);

			_vector_storeu(&block[i], w);

			m_decryptCounter += sizeof(vector_t);
		}

		i = len - (len % sizeof(vector_t));
#endif

		for (; i != len; ++i)
		{
			buf[i] ^= 0xAB;
			buf[i] = static_cast<uint8_t>(buf[i] << 4 | buf[i] >> 4);
			buf[i] ^= key1[(m_decryptCounter) & 0xFF];
			buf[i] ^= key2[(m_decryptCounter >> 8) & 0xFF];
			++m_decryptCounter;
		}
	}
}
