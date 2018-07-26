#pragma once
#ifndef __PACKET_HEADER__
#define __PACKET_HEADER__
// @nolimitk
// 13.09.12
// packet

#include "../NKCore.h"
#include "Protocol.h"

namespace NKNetwork
{
	class RecvStream;

	class Packet// : public NKCore::ByteStream
	{
		// for send by protobuf
	public:
		//byte *make(uint16_t command, uint16_t size);

		// for send
	public:
		/*template<typename T>
		T *set(uint16_t command);
		inline bool set(uint16_t command) { return set(command, 0); }
		template<typename T>
		bool write(T value);
		bool write(byte *value, uint16_t size);*/

	protected:
		//bool set(uint16_t command, uint16_t dataSize);

		// for read by external library
	public:
		inline byte *getData(void) const { if (getDataSize() == 0) { return nullptr; } else { return _pProtocolData; } }

		// for read
	public:
		/*template<typename T>
		T *getData(void);*/

	public:
		//inline bool isSystem(void) const { if (_pHeader != nullptr) { return (_pHeader->_system == (uint16_t)(PROTOCOLSYSTEM::SYSTEM)); } return false; }
		//inline PROTOCOLSIDE getSide(void) const { if (_pHeader != nullptr) { return (PROTOCOLSIDE)_pHeader->_side; } return PROTOCOLSIDE::REQ; }
		inline PROTOCOLHEAD::size_type getDataSize(void) const { if (_pHeader != nullptr) { return _pHeader->_size - sizeof(PROTOCOLHEAD); } return 0; }
		//inline uint16_t getCommandID(void) const { if (_pHeader != nullptr) { return _pHeader->_commandid; } return 0; }
		/*inline PACKETHEAD *getHeader(void) { return _pHeader; }*/

		// for receive
	public:
		bool set(RecvStream& stream);
				
	protected:
		// @nolimitk ����ü ������ ���� interface
		PROTOCOLHEAD *_pHeader;
		byte *_pProtocolData;

		//NKCore::Buffer _buffer;
				
	public:
		Packet(void);
		//Packet(NKCore::Buffer&& buffer);
		//Packet(Packet &param);
		virtual ~Packet(void);
	};

	//template<typename T>
	//T *Packet::set(uint16_t command)
	//{
	//	byte *data = Make(command, sizeof(T));
	//	if (data == nullptr)
	//	{
	//		return nullptr;
	//	}

	//	return reinterpret_cast<T*>(data);
	//}

	//template<typename T>
	//bool Packet::write(T value)
	//{
	//	if (getRemainSize() < sizeof(T)) return false;

	//	if (ByteStream::write<T>(value) == false)
	//	{
	//		return false;
	//	}

	//	// data size�� ������Ʈ �Ѵ�.
	//	_pHeader->_size += sizeof(T);
	//	return true;
	//}

	//template<typename T>
	//T *Packet::getData(void)
	//{
	//	if (getDataSize() < sizeof(T))
	//	{
	//		return nullptr;
	//	}
	//	else
	//	{
	//		pop(sizeof(T) + sizeof(PACKETHEAD));
	//		return reinterpret_cast<T*>(_pProtocolData);
	//	}
	//}
}

#endif // __PACKET_HEADER__