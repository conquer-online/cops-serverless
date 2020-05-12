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

#include "hook.h"

#include <cassert>
#include <cstring>
#include <limits>

namespace zfserver
{
	Hook::Hook() noexcept
		: m_originalAddress(nullptr), m_headerSize(0), m_thunkSize(0), m_thunk(nullptr)
	{

	}

	Hook::~Hook()
	{
		reset();
	}

	void Hook::redirect(LPVOID originalAddress, LPVOID targetAddress)
	{
		// Opcode: 'E9 cd' | Mnemonic: 'JMP rel32' | 5 bytes
		static constexpr size_t JMP_OPCODE_SIZE = 5;

		// ensure we haven't hooked anything yet...
		assert(m_originalAddress == nullptr);

		// determine the size of the leading instructions before the JMP
		m_headerSize = hookHeaderSize(originalAddress);
		assert(m_headerSize >= JMP_OPCODE_SIZE);

		// allocate a new thunk to be able to call the original function
		m_thunkSize = m_headerSize + JMP_OPCODE_SIZE; // leading instructions + JMP
		m_thunk = new BYTE[m_thunkSize];

		// make the thunk executable
		DWORD dwOld; // unused
		VirtualProtect(m_thunk, m_thunkSize, PAGE_EXECUTE_READWRITE, &dwOld);

		// write the thunk
		std::memcpy(m_thunk, originalAddress, m_headerSize);
		m_thunk[m_headerSize] = 0xE9; // JMP
		*((DWORD*)&m_thunk[m_headerSize + 1]) = relativeAddr32(&m_thunk[m_headerSize], static_cast<BYTE*>(originalAddress) + m_headerSize);

		// rewrite the original call to jump to the targetAddress instead
		BYTE patch[JMP_OPCODE_SIZE];
		patch[0] = 0xE9; // JMP
		*((DWORD*)&patch[1]) = relativeAddr32(originalAddress, targetAddress);
		std::memcpy(originalAddress, patch, sizeof(patch));
	}

	void Hook::reset() noexcept
	{
		// Opcode: 'E9 cd' | Mnemonic: 'JMP rel32' | 5 bytes
		static constexpr size_t JMP_OPCODE_SIZE = 5;

		if (m_thunk != nullptr)
		{
			// rewrite the bytes we overwritten from the thunk -- it still has the original ones
			std::memcpy(m_originalAddress, m_thunk, JMP_OPCODE_SIZE);
			delete[] m_thunk;
		}

		m_originalAddress = nullptr;

		m_headerSize = 0;
		m_thunkSize = 0;
		m_thunk = nullptr;
	}

	LPVOID Hook::thunk() const noexcept
	{
		return m_thunk;
	}

	int Hook::hookHeaderSize(LPVOID address)
	{
		// push ebp
		// mov ebp,esp
		// sub esp,#
		static BYTE FIRST_HEADER[] = { 0x55, 0x8B, 0xEC, 0x81, 0xEC };

		// mov edi,edi
		// push ebp
		// mov ebp,esp
		static BYTE SECOND_HEADER[] = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC };

		// jmp dword ptr [<&KERNEL32.GetLastError>] ; ntdll.RtlGetLastWin32Error (FF25 18111477)
		static BYTE THIRD_HEADER[] = { 0xFF, 0x25 };

		DWORD dwOld;
		VirtualProtect(address, 5, PAGE_EXECUTE_READWRITE, &dwOld);

		VirtualProtect(FIRST_HEADER, sizeof(FIRST_HEADER), PAGE_EXECUTE_READWRITE, &dwOld);
		VirtualProtect(SECOND_HEADER, sizeof(SECOND_HEADER), PAGE_EXECUTE_READWRITE, &dwOld);
		VirtualProtect(THIRD_HEADER, sizeof(THIRD_HEADER), PAGE_EXECUTE_READWRITE, &dwOld);

		if (std::memcmp(address, FIRST_HEADER, sizeof(FIRST_HEADER)) == 0)
			return 11;
		if (std::memcmp(address, SECOND_HEADER, sizeof(SECOND_HEADER)) == 0)
			return 5;
		if (std::memcmp(address, THIRD_HEADER, sizeof(THIRD_HEADER)) == 0)
			return 6;

		return 0;
	}

	DWORD Hook::relativeAddr32(LPVOID originalAddress, LPVOID targetAddress)
	{
		DWORD addr = (((DWORD)originalAddress - (DWORD)targetAddress) + sizeof(PVOID));
		return std::numeric_limits<DWORD>::max() - addr;
	}
}