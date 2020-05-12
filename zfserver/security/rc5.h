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

#ifndef ZFSERVER_SECURITY_RC5_H
#define ZFSERVER_SECURITY_RC5_H

#include <cstddef>
#include <cstdint>

namespace zfserver::security
{
    /**
     * @brief Rivest Cipher 5 (RC5)
     *
     * Rivest Cipher 5 (RC5) implementation for Conquer Online 2.0.
     * The game uses the RC5-32/12/16 version of the cipher.
     *
     * RC5 is a simple block cipher, designed in 1994 by Ronald Rivest.
     *
     * RC5 has a 32, 64 or 128-bit block size and a variable key length from
     * 1 bit up to 2040 bits. It is a Feistel-like cipher using between 1 to
     * 255 rounds. The suggested values are respectively 128, 64 and 12, although
     * 12-round RC5 (with 64-bit blocks) is susceptible to a differential attack
     * using 2 ^ 44 chosen plaintexts. To be still an effective cipher, 18-20 rounds
     * are now suggested.
     */
    class RC5 final
    {
    public:
        /** The size (in bytes) of the key. (0 - 255, 16 suggested) */
        static const size_t KEY_SIZE = 16;
        /** The size (in bytes) of the blocks, (4, 8 or 16, 8 suggested) */
        static const size_t BLOCK_SIZE = 8;
        /** The number of round of the algorithm (1 - 255, 12 suggested) */
        static const size_t ROUNDS = 12;

    public:
        /**
         * @brief Creates a new uninitialized RC5 cipher.
         */
        RC5() noexcept = default;

        /**
         * @brief Creates a new RC5 cipher and generates the key.
         *
         * @param[in]  seed  The seed to use to generate the key.
         */
        RC5(const uint8_t seed[KEY_SIZE]) noexcept;

        /* destructor */
        ~RC5() = default;

    public:
        /**
         * @brief Generates the key.
         *
         * @param[in]  seed  The seed to use to generate the key.
         */
        void generateKey(const uint8_t seed[KEY_SIZE]) noexcept;

        /**
         * @brief Encrypts the buffer.
         *
         * @param[in]  buf  The buffer to encrypt.
         * @param[in]  len  The length (in bytes) of the buffer.
         *
         * @remarks The encryption is done in-place.
         * @remarks The length must be a multiple of the BLOCK_SIZE.
         */
        void encrypt(uint8_t* buf, size_t len) noexcept;

        /**
         * @brief Decrypts the buffer.
         *
         * @param[in]  buf  The buffer to decrypt.
         * @param[in]  len  The length (in bytes) of the buffer.
         *
         * @remarks The decryption is done in-place.
         * @remarks The length must be a multiple of the BLOCK_SIZE.
         */
        void decrypt(uint8_t* buf, size_t len) noexcept;

    private:
        /**
         * @brief The size (in 32 bits block) of the key.
         */
        static constexpr size_t KEY32_SIZE = KEY_SIZE / sizeof(uint32_t);

        /**
         * @brief The size (in 32 bits block) of the sub key.
         */
        static constexpr size_t SUB32_SIZE = ((ROUNDS * 2) + 2);

    private:
        uint32_t m_sub[SUB32_SIZE]; //!< The generated sub key.
    };
}

#endif // ZFSERVER_SECURITY_RC5_H
