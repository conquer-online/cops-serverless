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

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

#include <windows.h>

using namespace std::literals;

int main(int argc, char* argv[])
{
	SetConsoleTitleW(L"Bootstrapping Conquer Online 2.0...");

	char workingDirectory[MAX_PATH];
	GetCurrentDirectoryA(sizeof(workingDirectory), workingDirectory);

	std::string commandLine;
	commandLine += workingDirectory;
	commandLine += '\\';
	commandLine += "conquer.exe"sv;

	commandLine += ' ';
	commandLine += "blacknull"sv;

	STARTUPINFOA startupInfo = {};
	PROCESS_INFORMATION processInfo = {};

	if (!CreateProcessA(NULL, const_cast<char*>(commandLine.c_str()), NULL, NULL, FALSE, 0, NULL, workingDirectory, &startupInfo, &processInfo))
	{
		std::cout << "Failed to start 'Conquer Online 2.0'." << std::endl;

		system("pause");
		return EXIT_FAILURE;
	}

	auto injectDLL = &injectDllWithLoadLibrary;
	if (argc >= 2)
	{
		if (argv[1] == "default"sv || argv[1] == "loadlibrary"sv)
			injectDLL = &injectDllWithLoadLibrary;
		else if (argv[1] == "loadlibrary_stub"sv)
			injectDLL = &injectDllWithLoadLibraryStub;
		else if (argv[1] == "ldrloaddll_stub"sv)
			injectDLL = &injectDllWithLdrLoadDllStub;
	}

	std::string dllName;
	dllName += workingDirectory;
	dllName += '\\';
	dllName += "zfserver.dll"sv;

	if (!injectDLL(processInfo.hProcess, dllName))
	{
		system("pause");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}