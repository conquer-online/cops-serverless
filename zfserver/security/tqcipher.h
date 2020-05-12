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

#ifndef ZFSERVER_SECURITY_TQ_CIPHER_H
#define ZFSERVER_SECURITY_TQ_CIPHER_H

#include <cstddef>
#include <cstdint>
#include <emmintrin.h>

namespace zfserver::security
{
	/**
	 * @brief TQ Cipher
	 *
	 * TQ Cipher implementation for Conquer Online 2.0.
	 *
	 * TQ Cipher is a simple symmetric stream cipher, probably designed by TQ Digital.
	 *
	 * TQ Cipher has a pre-shared key of 512 bits, and the possibility to generate
	 * an alternative key of 512 bits. The alternative key is only used for encrypting
	 * (on the client side) or decrypting (on the server side). The cipher uses
	 * two independent 16 bits counters to XOR a stream of data with the key.
	 *
	 * @remarks This implementation supports SSE2 and AVX2.
	 */
	class TqCipher final
	{
	public:
		/**
		 * @brief Creates a new TQ cipher using the Conquer Online pre-shared key.
		 */
		TqCipher() noexcept;

		/* destructor */
		~TqCipher() = default;

		/**
		 * @brief Generates the alternative key.
		 *
		 * @param[in]  a  The first 32 bits value to use as a seed.
		 * @param[in]  b  The second 32 bits value to use as a seed.
		 *
		 * @remarks A = Token, B = AccountUID in Conquer Online.
		 *
		 * @warning The decryption counter will be reset.
		 */
		void generateAltKey(int32_t a, int32_t b) noexcept;

		/**
		 * @brief Encrypts the buffer.
		 *
		 * @param[in]  buf  The buffer to encrypt.
		 * @param[in]  len  The length (in bytes) of the buffer.
		 *
		 * @remarks The encryption is done in-place.
		 */
		void encrypt(uint8_t* buf, size_t len) noexcept;

		/**
		 * @brief Decrypts the buffer.
		 *
		 * @param[in]  buf  The buffer to decrypt.
		 * @param[in]  len  The length (in bytes) of the buffer.
		 *
		 * @remarks The decryption is done in-place.
		 */
		void decrypt(uint8_t* buf, size_t len) noexcept;

	private:
#if defined(__AVX2__)
		using vector_t = __m256i;
		static constexpr size_t KEY_PADDING = sizeof(vector_t) - 1;
#elif defined(__SSE2__) || (defined(_M_IX86_FP) && _M_IX86_FP == 2)
		using vector_t = __m128i;
		static constexpr size_t KEY_PADDING = sizeof(vector_t) - 1;
#else
		static constexpr size_t KEY_PADDING = 0;
#endif

		/**
		 * @brief The size (in bytes) of the partial keys.
		 *
		 * @remarks The key is splitted in two parts.
		 */
		static constexpr size_t PARTIAL_KEY_SIZE = 256;

		/**
		 * @brief The size (in bytes) of the key.
		 *
		 * @remarks When using SIMD, the partial keys have padding.
		 */
		static constexpr size_t KEY_SIZE = (2 * PARTIAL_KEY_SIZE) + (2 * KEY_PADDING);

		/**
		 * @brief The offset of the second partial key.
		 */
		static constexpr size_t KEY_OFFSET = PARTIAL_KEY_SIZE + KEY_PADDING;

	private:
		uint8_t m_key[KEY_SIZE]; //!< The base key of the cipher.

		uint8_t m_altKey[KEY_SIZE]; //!< The alternative key of the cipher.
		bool m_usingAltKey = false; //!< Whether the alternative key should be used.

		uint16_t m_encryptCounter = 0; //!< The encryption counter.
		uint16_t m_decryptCounter = 0; //!< The decryption counter.
	};
}

#endif // ZFSERVER_SECURITY_TQ_CIPHER_H