#include "RecvStream.h"

using namespace NKNetwork;

RecvStream::RecvStream(const std::shared_ptr<NKCore::Buffer>& buffer)
	:ByteStream(buffer)
{
}

RecvStream::~RecvStream(void)
{
}

bool RecvStream::update(size_t length)
{
	if (_length > size() - length) return false;
	_length += length;
	return true;
}

bool RecvStream::moveRead(void)
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