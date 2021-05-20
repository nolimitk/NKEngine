#pragma once
#ifndef __RECVSTREAM_HEADER__
#define __RECVSTREAM_HEADER__
// @nolimitk
// 16.05.24
// receive stream

// usally include NKCore.h but it is header only
#include "../NKCore/ByteStream.h"
#include "../NKCore/Buffer.h"

namespace NKNetwork
{
	class RecvStream : public NKCore::ByteStream<NKCore::Buffer>
	{
	private:
		using ByteStream::write;

	public:
		// @write, when it write directly on buffer, update the length of buffer.
		inline bool update(size_t length)
		{
			if (_length > size() - length) return false;
			_length += length;
			return true;
		}

	public:
		bool moveRead(void)
		{
			if (_length < _offset) return false;
			if (_offset == 0) return true;
			if (_length > _offset)
			{
				memmove(get(), get() + _offset, _length - _offset);
			}
			_length -= _offset;
			_offset = 0;
			return true;
		}

	public:
		inline NKCore::byte* getRemainBuffer(void) const { return get() + _length; }
		inline size_t getRemainSize(void) const { return size() - _length; }
		
	public:
		RecvStream(size_t size)
			:ByteStream(size)
		{
		}
		virtual ~RecvStream(void)
		{
		}
	};
}

#endif // __RECVSTREAM_HEADER__