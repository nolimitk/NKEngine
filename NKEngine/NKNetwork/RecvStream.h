#pragma once
#ifndef __RECVSTREAM_HEADER__
#define __RECVSTREAM_HEADER__
// @nolimitk
// 16.05.24
// receive stream

#include "../NKCore.h"

namespace NKNetwork
{
	class RecvStream : public NKCore::ByteStream
	{
	private:
		ByteStream::write;

	public:
		// @write, when it write directly on buffer, update the length of buffer.
		bool update(size_t length);

	public:
		bool moveRead(void);

	public:
		inline byte* getRemainBuffer(void) const { return get() + _length; }
		inline size_t getRemainSize(void) const { return size() - _length; }
		
	public:
		RecvStream(const std::shared_ptr<NKCore::Buffer>& buffer);
		virtual ~RecvStream(void);
	};
}

#endif // __RECVSTREAM_HEADER__