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

#include "injectors.h"

#include <iostream>
#include <string>

#include <windows.h>

using SMART_HANDLE = internal::SMART_HANDLE;

namespace
{
	struct StubData
	{
		LPVOID LoadLibraryW;
		LPVOID GetLastError;
		wchar_t DllName[MAX_PATH];
	};

// disable runtime checks -- we want the stub to not rely on anything from the injector
#pragma strict_gs_check(push, off)
#pragma runtime_checks("", off)
	__declspec(noinline) static DWORD WINAPI LoadLibraryStub(LPVOID parameter)
	{
		StubData& stubData = *reinterpret_cast<StubData*>(parameter);

		using LP_LoadLibraryW = HMODULE(WINAPI*)(LPCWSTR lpLibFileName);
		using LP_GetLastError = DWORD(WINAPI*)();

		auto LoadLibraryW = reinterpret_cast<LP_LoadLibraryW>(stubData.LoadLibraryW);
		auto GetLastError = reinterpret_cast<LP_GetLastError>(stubData.GetLastError);

		HMODULE moduleHandle = LoadLibraryW(stubData.DllName);
		return moduleHandle != nullptr ? EXIT_SUCCESS : GetLastError();
	}

	// indicator to find the size of the stub without looking at the ASM
	__declspec(noinline) static DWORD WINAPI LoadLibraryStubEnd()
	{
		return 0;
	}
#pragma runtime_checks("", restore)
#pragma strict_gs_check(pop)
}

bool injectDllWithLoadLibraryStub(HANDLE processHandle, const std::string& dllName)
{
	const size_t stubSize = reinterpret_cast<uintptr_t>(&LoadLibraryStubEnd) - reinterpret_cast<uintptr_t>(&LoadLibraryStub);

	std::cout << "Injecting DLL '" << dllName << "' inside " << processHandle << std::endl;
	SIZE_T writtenBytes = 0;

	std::cout << "Looking for address of function 'LoadLibraryW' in 'kernel32.dll' ... ";
	PVOID loadLibrary = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryW");
	if (loadLibrary == nullptr)
	{
		std::cout << "Failed!" << std::endl;
		std::cout << "Could not find the address of the function 'LoadLibraryW' in 'kernel32.dll'." << std::endl;
		return false;
	}
	std::cout << "Found at " << loadLibrary << "." << std::endl;

	std::cout << "Looking for address of function 'GetLastError' in 'kernel32.dll' ... ";
	PVOID getLastError = GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetLastError");
	if (getLastError == nullptr)
	{
		std::cout << "Failed!" << std::endl;
		std::cout << "Could not find the address of the function 'GetLastError' in 'kernel32.dll'." << std::endl;
		return false;
	}
	std::cout << "Found at " << getLastError << "." << std::endl;

	StubData stubData = {};
	stubData.LoadLibraryW = loadLibrary;
	stubData.GetLastError = getLastError;
	MultiByteToWideChar(CP_ACP, 0, dllName.data(), dllName.size() + 1, stubData.DllName, sizeof(stubData.DllName) / sizeof(stubData.DllName[0]));

	std::cout << "Allocating memory for the stub data within the virtual address space of the process ... ";
	PVOID paramAddr = VirtualAllocEx(processHandle, NULL, sizeof(stubData), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (paramAddr == nullptr)
	{
		std::cout << "Failed!" << std::endl;
		std::cout << "Could not allocate " << sizeof(stubData) << " bytes of memory within the virtual address space of the process." << std::endl;
		return false;
	}
	std::cout << "Allocated at " << paramAddr << "." << std::endl;

	std::cout << "Writing stub data inside the virtual address space of the process ... ";
	writtenBytes = 0;
	if (!WriteProcessMemory(processHandle, paramAddr, &stubData, sizeof(stubData), &writtenBytes))
	{
		std::cout << "Failed!" << std::endl;
		std::cout << "Could not write the stub data inside the virtual address space of the target process." << std::endl;
		return false;
	}
	std::cout << "Wrote " << writtenBytes << " bytes." << std::endl;

	std::cout << "Allocating memory for the stub within the virtual address space of the process ... ";
	PVOID stubAddr = VirtualAllocEx(processHandle, NULL, stubSize, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (stubAddr == nullptr)
	{
		std::cout << "Failed!" << std::endl;
		std::cout << "Could not allocate " << stubSize << " bytes of memory within the virtual address space of the process." << std::endl;
		return false;
	}
	std::cout << "Allocated at " << stubAddr << "." << std::endl;

	std::cout << "Writing stub inside the virtual address space of the process ... ";
	writtenBytes = 0;
	if (!WriteProcessMemory(processHandle, stubAddr, &LoadLibraryStub, stubSize, &writtenBytes))
	{
		std::cout << "Failed!" << std::endl;
		std::cout << "Could not write the stub inside the virtual address space of the target process." << std::endl;
		return false;
	}
	std::cout << "Wrote " << writtenBytes << " bytes." << std::endl;

	std::cout << "Creating a thread inside the target process ... ";
	HANDLE threadHandle = CreateRemoteThread(processHandle, NULL, 0, (LPTHREAD_START_ROUTINE)stubAddr, paramAddr, 0, NULL);
	SMART_HANDLE smartThreadHandle = SMART_HANDLE{ threadHandle }; // auto release the handle
	if (threadHandle == nullptr)
	{
		std::cout << "Failed!" << std::endl;
		std::cout << "Could not create a thread inside the target process." << std::endl;
		return false;
	}
	std::cout << "Thread " << threadHandle << " created." << std::endl;

	std::cout << "Waiting for the thread ... ";
	DWORD result = WaitForSingleObject(threadHandle, 10'000); // time-out after 10s
	if (WAIT_ABANDONED == result || WAIT_TIMEOUT == result || WAIT_FAILED == result)
	{
		std::cout << "Failed!" << std::endl;
		std::cout << "Time-out while waiting for the created thread." << std::endl;
		return false;
	}
	std::cout << "Done" << "." << std::endl;

	std::cout << "Checking for the injected DLL ... ";
	DWORD exitCode = 0;
	if (GetExitCodeThread(threadHandle, &exitCode))
	{
		if (exitCode != 0)
		{
			std::cout << "Failed!" << std::endl;
			std::cout << "Thread exited with code " << exitCode << "." << std::endl;
			return false;
		}
	}
	else
	{
		std::cout << "Failed!" << std::endl;
		std::cout << "Cannot retrieve exit code of the thread." << std::endl;
		return false;
	}
	std::cout << "Done" << "." << std::endl;

	// Sleep thread for 3s
	Sleep(3'000);

	return true;
}