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

#ifndef BOOTSTRAP_INJECTORS_H
#define BOOTSTRAP_INJECTORS_H

#include <memory>
#include <string>
#include <type_traits>

#include <windows.h>

namespace internal
{
	struct HandleDeleter
	{
		void operator()(HANDLE handle)
		{
			if (handle != nullptr)
				CloseHandle(handle);
		}
	};

	using SMART_HANDLE = std::unique_ptr<std::remove_pointer_t<HANDLE>, HandleDeleter>;
}

/**
\brief Inject the specified DLL into the process using the regular LoadLibrary trick.

\details This method abuses from the fact that LoadLibrary takes a single argument and return a HANDLE
	     (which can be reinterpreted as a regular integer). This signature matches the required signature
		 for LPTHREAD_START_ROUTINE, as such, a remote thread can be created with LoadLibrary
		 as the routine to execute. To detect whether the injection worked, the exit code can be checked.
		 As loaded modules are always loaded in the lower 32 bits (even on 64 bits platforms), a non-zero
		 result means that the module was loaded successfully while zero means that NULL was returned.

		 This method:
		 - find the address of LoadLibraryA in the current process (even with ASLR, it works because the randomization is per session)
		 - allocate read/write memory page inside the target process
		 - write the DLL name in that memory page
		 - create a remote thread inside the target process with LoadLibraryA as the routine, and the allocated memory page as the parameter
		 - wait and check for the remote thread the exit code

		 Note. To work, both the injector and the target process must be of the same architecture (e.g. both x86 or both x64).

\remarks If the injection fails, there is no way to know what was the actual error.
*/
bool injectDllWithLoadLibrary(HANDLE processHandle, const std::string& dllName);

/**
\brief Inject the specified DLL into the process using a stub calling LoadLibrary (kernel32).

\details This method implement a stub in the injector that will be written in the target process and executed.
		 That stub will call LoadLibraryW and is able to retrieve the error by calling GetLastError too.
		 Note. For the stub to work, it must not rely on any addresses from the injector. It means we must be really when we write it.

		 This method:
		 - find the address of LoadLibraryW in the current process (even with ASLR, it works because the randomization is per session)
		 - find the address of GetLastError in the current process (even with ASLR, it works because the randomization is per session)
		 - allocate read/write memory page inside the target process
		 - write the stub data (function addresses, DLL name) in that memory page
		 - allocate an executable memory page inside the target process
		 - write the stub code (assembler) in that memory page
		 - create a remote thread inside the target process with allocated stub code as the routine, and the allocated stub data as the parameter
		 - wait and check for the remote thread the exit code

\remarks There is no official way to retrieve the size of a function in ASM.

		 I used a trick which is to force the compiler to create static non-inlinable functions next to each others
		 and compute the size with the difference between the addresses.

		 An alternative would be to parse the ASM at runtime and find the 0xCC instruction.
*/
bool injectDllWithLoadLibraryStub(HANDLE processHandle, const std::string& dllName);

/**
\brief Inject the specified DLL into the process using a stub calling the undocumented LdrLoadDll (ntdll).

\details This method implement a stub in the injector that will be written in the target process and executed.
		 That stub will call LdrLoadDll and is able to retrieve the error by calling RtlGetLastWin32Error too.
		 Note. For the stub to work, it must not rely on any addresses from the injector. It means we must be really when we write it.

		 This method:
		 - find the address of LdrLoadDll in the current process (even with ASLR, it works because the randomization is per session)
		 - find the address of RtlInitUnicodeString in the current process (even with ASLR, it works because the randomization is per session)
		 - find the address of RtlGetLastWin32Error in the current process (even with ASLR, it works because the randomization is per session)
		 - allocate read/write memory page inside the target process
		 - write the stub data (function addresses, DLL name) in that memory page
		 - allocate an executable memory page inside the target process
		 - write the stub code (assembler) in that memory page
		 - create a remote thread inside the target process with allocated stub code as the routine, and the allocated stub data as the parameter
		 - wait and check for the remote thread the exit code

\remarks There is no official way to retrieve the size of a function in ASM.

		 I used a trick which is to force the compiler to create static non-inlinable functions next to each others
		 and compute the size with the difference between the addresses.

		 An alternative would be to parse the ASM at runtime and find the 0xCC instruction.
*/
bool injectDllWithLdrLoadDllStub(HANDLE processHandle, const std::string& dllName);

#endif // BOOTSTRAP_INJECTORS_H