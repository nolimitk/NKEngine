#pragma once
#ifndef __BYTESTREAM_HEADER__
#define __BYTESTREAM_HEADER__
// @nolimitk
// 16.05.10
// byte stream

#include "NKDataTypes.h"
#include "Buffer.h"
#include <memory>

namespace NKCore
{
	class Buffer;

	class ByteStream
	{
	public:
		inline size_t getLength(void) const { return _length - _offset; }

	public:
		template<typename T>
		T peek(void);
		bool peek(byte** pTarget, size_t size);

		template<typename T>
		T read(void);
		bool read(byte** pTarget, size_t size);

		template<typename T>
		bool write(T value);
		bool write(byte* value, size_t size);

	protected:
		inline byte* get(void) const { return _buffer.get()->get(); }
		inline size_t size(void) const { return _buffer.get()->_size; }
				
	protected:
		std::shared_ptr<Buffer> _buffer;
		size_t _length; // length of written
		size_t _offset; // offset of read
		
	public:
		ByteStream(const std::shared_ptr<Buffer>& buffer);
		virtual ~ByteStream(void);
	};

	template<typename T>
	T ByteStream::peek(void)
	{
		if (_length < _offset ||
			sizeof(T) > _length - _offset ||
			_buffer == nullptr) return (T)0;
		T val;
		val = *(T *)(get() + _offset);
		return val;
	}

	template<typename T>
	T ByteStream::read(void)
	{
		T val = peek<T>();
		_offset += sizeof(T);
		return val;
	}

	template<typename T>
	bool ByteStream::write(T value)
	{
		if (_length > size() - sizeof(T)) return false;

		T *pCurrent = (T *)(get() + _length);
		*pCurrent = value;

		_length += sizeof(T);
		return true;
	}
}

#endif // __BYTESTREAM_HEADER__