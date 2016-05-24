#pragma once
#ifndef __SENDSTREAM_HEADER__
#define __SENDSTREAM_HEADER__
// @nolimitk
// 16.05.24
// send stream

#include "../NKCore.h"

namespace NKNetwork
{
	class SendStream : public NKCore::ByteStream
	{
	private:
		ByteStream::peek;
		ByteStream::read;
		ByteStream::size;

	public:
		inline byte* get(void) const { return ByteStream::get(); }

	public:
		SendStream(const std::shared_ptr<NKCore::Buffer>& buffer);
		virtual ~SendStream(void);
	};
}

#endif // __SENDSTREAM_HEADER__