# COPS Serverless

Copyright (C) 2020 CptSky <br />
https://gitlab.com/conquer-online/servers/cops-serverless

## Overview
COPS serverless is a custom DLL mimicking an in-process server and targeting the version 4330 of Conquer Online 2.0. It redirects WinSock2 calls to act as the server without ever exiting the game process.

## History
A proof of concept was originally shown in May 2014. The original project was derived from the protection DLL used for COPS v6 - The Return Of The Legend. The source code of the POC was never saved. In May 2020, it was redevelopped from unreleased projects following a release request.

## Features

COPS serverless only implements the basic messages required for the minimal login sequence. The library implements:
- Basic hooking class for x86
- Security classes based on unreleased work (supporting SSE2/AVX2 in a single implementation)
- Message classes based on COPS v7 (modernized for C++17)
- Hooking of WinSock2 functions to intercept network calls
- Minimal login sequence of Conquer Online

<br />
COPS serverless also provides a bootstrap executable to launch the game and inject the server at startup. The injector initially supported only one injection method, but two others were developed as the first one wasn't working at first (there was a typo in the project name of the DLL... as such the DLL was not found).

The injection method can be **optionally** selected by passing the method as the argument of bootstrap:
- **loadlibrary**: Execute LoadLibraryA from kernel32
- **loadlibrary_stub**: Execute a stub calling LoadLibraryW from kernel32
- **ldrloaddll_stub**: Execute a stub calling LdrLoadDll from ntdll

## Supported systems

The library was developed using Visual Studio 2019 and requires a C++17 compiler. It only supports Microsoft Windows (32-bit) like the original Conquer Online 2.0 game client.