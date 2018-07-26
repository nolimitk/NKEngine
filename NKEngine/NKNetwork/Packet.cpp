#include "Packet.h"
#include "RecvStream.h"

using namespace NKNetwork;
using namespace std;

NKNetwork::Packet::Packet(void)
	:_pHeader(nullptr)
	, _pProtocolData(nullptr)
{
}

//Packet::Packet(NKCore::Buffer&& buffer)
//	:_buffer(std::move(buffer))
//	,_pHeader(nullptr)
//	,_pProtocolData(nullptr)
//{
//	//__GUARD__;
//
//	//__UNGUARD__;
//}
//
//Packet::Packet(Packet &param)
//	: _buffer(std::move(param._buffer))
//	,_pHeader(param._pHeader)
//	,_pProtocolData(param._pProtocolData)
//{
//}

Packet::~Packet(void)
{
	//__GUARD__;
	
	//__UNGUARD__;
}

//byte *Packet::make(uint16_t command, uint16_t size)
//{
//	if( set( command, size ) == false )
//	{
//		return NULL;
//	}
//
//	_pProtocolData = getRemainBuffer();
//
//	// pointer에 직접 write를 하므로 bytestream은 이미 모두 write했다고 설정한다. [2014/10/2/ nolimitk]
//	if( update(size) == false )
//	{
//		return nullptr;
//	}
//
//	return _pProtocolData;
//}

//bool Packet::write(byte *value, uint16_t size)
//{
//	if( size == 0 ) return false;
//	if( getRemainSize() < size ) return false;
//	if( value == nullptr ) return false;
//
//	if( ByteStream::write( value, size ) == false )
//	{
//		return false;
//	}
//
//	// data size를 업데이트 한다.
//	_pHeader->_size += size;
//	return true;
//}

//bool Packet::set(uint16_t command, uint16_t dataSize)
//{
//	if( sizeof(command) + sizeof(_pHeader->_size) + dataSize > getRemainSize() ) return false;
//
//	reset();
//
//	write<uint16_t>(sizeof(command) + dataSize);
//	write<uint16_t>(command);
//
//	_pHeader = (PACKETHEAD *)getBuffer();
//	_pProtocolData = nullptr;
//	
//	return true;
//}

bool NKNetwork::Packet::set(RecvStream & stream)
{
	if (stream.read((byte**)&_pHeader, sizeof(PROTOCOLHEAD)) == false) return false;
	if (getDataSize() > 0)
	{
		if (stream.read(&_pProtocolData, getDataSize()) == false) return false;
	}
	return true;
}