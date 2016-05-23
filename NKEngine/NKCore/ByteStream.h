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

	//class ByteStream
	//{
	//public:
	//	inline byte *getBuffer(void) { return _pStream; }
	//	inline int getRemainLength(void) { return _length - _offset; }
	//	inline uint getLength(void) { return _length; }
	//	inline BYTE *getRemainBuffer(void) { return _pStream + _length; }
	//	inline uint getRemainSize(void) { return _size - _length; }
	//	inline uint getSize(void) { return _size; }

	//public:
	//	void init(byte *pBuffer, uint size);
	//	void reset(void);

	//	bool update(uint length); // buffer에 직접 write를 했을 경우 length를 수정해주기 위해 사용
	//	bool pop(uint size); // buffer에서 직접 read 했거나 pass할 경우 사용

	//	bool moveRead(void);

	//	template<typename T>
	//	T peek(void);
	//	bool peek(byte **pTarget, int size);

	//	template<typename T>
	//	T read(void);
	//	bool read(byte **pTarget, int size);

	//	template<typename T>
	//	bool write(T value);
	//	bool write(byte *value, int size);

	//	bool copy(byte* value, uint size);
	//	bool copy(ByteStream* pSource);

	//protected:
	//	byte* _pStream;
	//	uint _size; // size of stream
	//	uint _length; // length of written
	//	uint _offset; // offset of read


	//public:
	//	ByteStream(void);
	//	ByteStream(byte *pBuffer, uint size);
	//	virtual ~ByteStream(void);
	//};

	//template<typename T>
	//T ByteStream::peek(void)
	//{
	//	if (_offset > _length - sizeof(T)) return (T)0;
	//	T val;
	//	val = *(T *)(_pStream + _offset);
	//	return val;
	//}

	//template<typename T>
	//T ByteStream::read(void)
	//{
	//	T val = peek<T>();
	//	_offset += sizeof(T);
	//	return val;
	//}

	//template<typename T>
	//bool ByteStream::write(T value)
	//{
	//	if (_length > _size - sizeof(T)) return false;

	//	T *pCurrent = (T *)(_pStream + _length);
	//	*pCurrent = value;

	//	_length += sizeof(T);
	//	return true;
	//}

	///

	class ReadStream : public ByteStream
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
		ReadStream(const std::shared_ptr<Buffer>& buffer);
		virtual ~ReadStream(void);
	};
	
	class WriteStream
	{
	public:
		inline byte* getBuffer(void) const { return _buffer; }
		inline size_t getLength(void) const { return _length - _offset; }

	public:
		bool write(byte* value, size_t size);

	protected:
		byte* _buffer;
		size_t _size; // size of stream
		size_t _length; // length of written
		size_t _offset; // offset of read

	public:
		WriteStream(const Buffer& buffer);
		virtual ~WriteStream(void);
	};
}

#endif // __BYTESTREAM_HEADER__