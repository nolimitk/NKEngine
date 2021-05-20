#pragma once
#ifndef __BYTESTREAM_HEADER__
#define __BYTESTREAM_HEADER__
// @nolimitk
// 16.05.10
// byte stream

#include <memory>
#include "NKCoreDataTypes.h"

namespace NKCore
{
	template<class BufferType>
	class ByteStream
	{
	public:
		inline size_t getLength(void) const { return _length - _offset; }

	public:
		template<class T>
		T peek(void);
		bool peek(byte** pTarget, size_t size);

		template<class T>
		T read(void);
		bool read(byte** pTarget, size_t size);

		template<class T>
		bool write(T value);
		bool write(byte* value, size_t size);

	protected:
		inline byte* get(void) const { return _buffer.get()->get(); }
		inline size_t size(void) const { return _buffer.get()->size(); }
				
	protected:
		std::shared_ptr<BufferType> _buffer;
		size_t _length; // length of written
		size_t _offset; // offset of read
		
	public:
		ByteStream(size_t size)
			:_buffer(std::make_shared <BufferType>(size))
			, _length(0)
			, _offset(0)
		{
		}
		virtual ~ByteStream(void)
		{

		}
	};

	template<class BufferType>
	bool ByteStream<BufferType>::peek(byte** pTarget, size_t size)
	{
		if (_length < _offset ||
			size > _length - _offset ||
			_buffer == nullptr ||
			pTarget == nullptr) return false;
		*pTarget = get() + _offset;
		return true;
	}

	template<class BufferType>
	template<class T>
	T ByteStream<BufferType>::peek(void)
	{
		if (_length < _offset ||
			sizeof(T) > _length - _offset ||
			_buffer == nullptr) return (T)0;
		T val;
		val = *(T *)(get() + _offset);
		return val;
	}

	template<class BufferType>
	bool ByteStream<BufferType>::read(byte** pTarget, size_t size)
	{
		bool ret = peek(pTarget, size);
		if (ret == true) _offset += size;
		return ret;
	}

	template<class BufferType>
	bool ByteStream<BufferType>::write(byte* val, size_t size)
	{
		if (_length > ByteStream::size() - size ||
			_buffer == nullptr ||
			val == nullptr) return false;

		std::copy(val, val+size, get() + _length);
		_length += size;
		return true;
	}

	template<class BufferType>
	template<class T>
	T ByteStream<BufferType>::read(void)
	{
		T val = peek<T>();
		_offset += sizeof(T);
		return val;
	}

	template<class BufferType>
	template<class T>
	bool ByteStream<BufferType>::write(T value)
	{
		if (_length > size() - sizeof(T)) return false;

		T *pCurrent = (T *)(get() + _length);
		*pCurrent = value;

		_length += sizeof(T);
		return true;
	}
}

#endif // __BYTESTREAM_HEADER__