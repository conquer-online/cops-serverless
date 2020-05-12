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

#ifndef ZFSERVER_HOOK_H
#define ZFSERVER_HOOK_H

#include <windows.h>

namespace zfserver
{
	class Hook final
	{
	public:
		Hook() noexcept;
		~Hook();

		Hook(Hook&&) = delete;
		Hook(const Hook&) = delete;
		Hook& operator=(Hook&&) = delete;
		Hook& operator=(const Hook&) = delete;

		void redirect(LPVOID originalAddress, LPVOID targetAddress);
		void reset() noexcept;

		LPVOID thunk() const noexcept;

	private:
		static int hookHeaderSize(LPVOID address);
		static DWORD relativeAddr32(LPVOID originalAddress, LPVOID targetAddress);

	private:
		LPVOID m_originalAddress; //!< the address of the hooked function

		SIZE_T m_headerSize; //!< the size of the trailing instructions
		SIZE_T m_thunkSize; //!< the size of the thunk (including the header)
		PBYTE m_thunk; //!< the thunk -- a small function used to jump to the real function
	};
}

#endif // ZFSERVER_HOOK_H