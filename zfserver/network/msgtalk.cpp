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

#include "msgtalk.h"

#include "log.h"
#include "network/stringpacker.h"

#include <cassert>

#include <windows.h>

using namespace std::literals;

namespace zfserver::network
{
	MsgTalk::MsgTalk(std::string_view speaker, std::string_view hearer, std::string_view words, Channel channel, Color color)
		: Msg(sizeof(MsgInfo) +
			1 + speaker.size() +
			1 + hearer.size() +
			1 + /* emotion.size() + */
			1 + words.size())
		, m_info(bufferAs<MsgInfo>())
	{
		create(speaker, hearer, ""sv, words, channel, color);
	}

	MsgTalk::MsgTalk(const uint8_t* buf, const size_t len)
		: Msg(buf, len), m_info(bufferAs<MsgInfo>())
	{
		assert(len >= sizeof(MsgInfo));
	}

	MsgTalk::MsgTalk(MsgTalk&& other) noexcept
		: Msg(std::move(other)), m_info(bufferAs<MsgInfo>())
	{

	}

	MsgTalk::MsgTalk(const MsgTalk& other)
		: Msg(other), m_info(bufferAs<MsgInfo>())
	{

	}

	MsgTalk& MsgTalk::operator=(MsgTalk&& other) noexcept
	{
		Msg::operator=(std::move(other));
		m_info = bufferAs<MsgInfo>();

		return *this;
	}

	MsgTalk& MsgTalk::operator=(const MsgTalk& other)
	{
		Msg::operator=(other);
		m_info = bufferAs<MsgInfo>();

		return *this;
	}

	void MsgTalk::create(std::string_view speaker, std::string_view hearer, std::string_view emotion, std::string_view words, Channel channel, Color color)
	{
		assert(speaker.size() < MAX_NAMESIZE);
		assert(hearer.size() < MAX_NAMESIZE);
		assert(emotion.size() < MAX_NAMESIZE);
		assert(words.size() < MAX_WORDSSIZE);

		m_info->Header.Length = static_cast<uint16_t>(length());
		m_info->Header.Type = MSG_TALK;

		m_info->Color = color;
		m_info->Channel = channel;
		m_info->Style = Style::Normal;
		m_info->Timestamp = GetTickCount();

		StringPacker packer(m_info->StringPack);
		packer.addString(speaker);
		packer.addString(hearer);
		packer.addString(emotion);
		packer.addString(words);
	}

	void MsgTalk::process(Client& client, Connection& connection)
	{
		StringPacker packer(m_info->StringPack);
		auto speaker = packer.getString(0);
		auto hearer = packer.getString(1);
		auto words = packer.getString(3);

		assert(speaker.has_value());
		assert(hearer.has_value());
		assert(words.has_value());

		LOG(DBG, "%s said %s to %s", std::string{ *speaker }.c_str(), std::string{ *words }.c_str(), std::string{ *hearer }.c_str());
	}
}