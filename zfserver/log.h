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

#ifndef ZFSERVER_LOG_H
#define ZFSERVER_LOG_H

#include <cstdio>
#include <cstring>

// Note. This is really awful/slow logger. But it helps debugging.

// __FILENAME__ is a suggested macro, until it exists, using __FILE__
#ifndef __FILENAME__
#   define __FILENAME__ (std::strrchr(__FILE__, '\\') ? std::strrchr(__FILE__, '\\') + 1 : __FILE__)
#endif // __FILENAME__

static FILE* logfile = fopen("./log.txt", "at");

// temp
#ifdef _NDEBUG
#define LOG__(LEVEL, FILE, FUNCTION, LINE, FORMAT, ...) \
    if (LEVEL > LogLevel::LOG_LEVEL_DBG) \
        std::fprintf(logfile, "[%s:%s:%u] ", FILE, FUNCTION, LINE), std::fprintf(logfile, FORMAT, ## __VA_ARGS__), std::fprintf(logfile, "\n"), std::fflush(logfile)
#else
#define LOG__(LEVEL, FILE, FUNCTION, LINE, FORMAT, ...) \
    if (LEVEL >= LogLevel::LOG_LEVEL_DBG) \
        std::fprintf(logfile, "[%s:%s:%u] ", FILE, FUNCTION, LINE), std::fprintf(logfile, FORMAT, ## __VA_ARGS__), std::fprintf(logfile, "\n"), std::fflush(logfile)
#endif // _NDEBUG

#define LOG_(LEVEL, ...) \
    LOG__(LEVEL, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__)

#define LOG(LEVEL, ...) LOG_(LogLevel::LOG_LEVEL_ ## LEVEL, __VA_ARGS__)


enum class LogLevel
{
	LOG_LEVEL_VRB = 0,
	LOG_LEVEL_DBG = 1,
	LOG_LEVEL_INFO = 2,
	LOG_LEVEL_WARN = 3,
	LOG_LEVEL_ERROR = 4,
	LOG_LEVEL_CRIT = 5
};

#endif // ZFSERVER_LOG_H
