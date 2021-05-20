#pragma once
#ifndef __SENDSTREAM_HEADER__
#define __SENDSTREAM_HEADER__
// @nolimitk
// 16.05.24
// send stream

// usally include NKCore.h but it is header only
#include "../NKCore/ByteStream.h"
#include "../NKCore/Buffer.h"

namespace NKNetwork
{
	class SendStream : public NKCore::ByteStream<NKCore::Buffer>
	{
	private:
		using ByteStream::peek;
		using ByteStream::read;
		using ByteStream::size;

	public:
		inline NKCore::byte* get(void) const { return ByteStream::get(); }

	public:
		SendStream(size_t size)
			:ByteStream(size)
		{
		}
		virtual ~SendStream(void)
		{
		}
	};
}

#endif // __SENDSTREAM_HEADER__