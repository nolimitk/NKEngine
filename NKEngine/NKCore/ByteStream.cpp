#include "ByteStream.h"

using namespace NKCore;

ByteStream::ByteStream(const std::shared_ptr<Buffer>& buffer)
	:_buffer(buffer)
	, _length(0)
	, _offset(0)
{
}

ByteStream::~ByteStream(void)
{
}

bool ByteStream::peek(byte** pTarget, size_t size)
{
	if (_length < _offset ||
		size > _length - _offset ||
		_buffer == nullptr ||
		pTarget == nullptr) return false;
	*pTarget = get() + _offset;
	return true;
}

bool ByteStream::read(byte** pTarget, size_t size)
{
	bool ret = peek(pTarget, size);
	if (ret == true) _offset += size;
	return ret;
}

bool ByteStream::write(byte* val, size_t size)
{
	if (_length > ByteStream::size() - size ||
		_buffer == nullptr ||
		val == nullptr) return false;

	memcpy_s(get() + _length, ByteStream::size() - _length, val, size);
	_length += size;
	return true;
}