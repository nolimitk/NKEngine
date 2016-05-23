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

//ByteStream::ByteStream(void)
//	:_pStream(nullptr)
//	,_size(0)
//	,_length(0)
//	,_offset(0)
//{
//}
//
//ByteStream::ByteStream(byte *pBuffer, uint size)
//	:_pStream(pBuffer)
//	,_size(size)
//	,_length(0)
//	,_offset(0)
//{
//}
//
//ByteStream::~ByteStream(void)
//{
//}
//
//void ByteStream::init(byte *pBuffer, uint size)
//{
//	_pStream = pBuffer;
//	_offset = 0;
//	_length = 0;
//	_size = size;
//}
//
//void ByteStream::reset(void)
//{
//	_offset = 0;
//	_length = 0;
//}
//
//bool ByteStream::update(uint length)
//{
//	if( _length > _size - length ) return false;
//	_length += length;
//	return true;
//}
//
//bool ByteStream::pop(uint size)
//{
//	if( _offset > _length - size ) return false;
//	_offset += size;
//	return true;
//}
//
//bool ByteStream::moveRead(void)
//{
//	// @TODO memmove를 하지 않고 circluar buffer를 구현한다.
//	if( _length < _offset ) return false;
//	if( _offset == 0 ) return true;
//	memmove( _pStream, _pStream+_offset, _length-_offset );
//	_length -= _offset;
//	_offset = 0;
//	return true;
//}
//
//bool ByteStream::peek(byte **pTarget, int size)
//{
//	if( size > (int)(_length - _offset) ||
//		_pStream == nullptr ||
//		pTarget == nullptr ) return false;
//	*pTarget = _pStream+_offset;
//	return true;
//}
//
//bool ByteStream::read(byte **pTarget, int size)
//{
//	bool ret = peek(pTarget, size);
//	if( ret == true ) _offset += size;
//	return ret;
//}
//
//bool ByteStream::write(byte *val, int size)
//{
//	if( _length > _size - size ||
//		_pStream == nullptr ||
//		val == nullptr ) return false;
//	
//	// @TODO memcpy를 사용하지 않고 pointer 연산으로만 구현한다.
//	memcpy_s( _pStream+_length, _size-_length, val, size );
//	_length += size;
//	return true;
//}
//
//bool ByteStream::copy(byte* value, uint size)
//{
//	reset();
//	return write( value, size );
//}
//
//bool ByteStream::copy(ByteStream* pSource)
//{
//	if( pSource == nullptr ) return false;
//	reset();
//	return write( pSource->getBuffer(), pSource->getLength() );
//}

ReadStream::ReadStream(const std::shared_ptr<Buffer>& buffer)
	:ByteStream(buffer)
{
}

ReadStream::~ReadStream(void)
{
}

bool ReadStream::update(size_t length)
{
	if (_length > size() - length) return false;
	_length += length;
	return true;
}

bool ReadStream::moveRead(void)
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


WriteStream::WriteStream(const Buffer& buffer)
	:_buffer(buffer.get())
	, _size(buffer._size)
	, _length(0)
	, _offset(0)
{
}

WriteStream::~WriteStream(void)
{
}

bool WriteStream::write(byte* val, size_t size)
{
	if (_length > _size - size ||
		_buffer == nullptr ||
		val == nullptr) return false;

	memcpy_s(_buffer + _length, _size - _length, val, size);
	_length += size;
	return true;
}