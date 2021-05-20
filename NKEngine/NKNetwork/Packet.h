#pragma once
#ifndef __PACKET_HEADER__
#define __PACKET_HEADER__
// @nolimitk
// 13.09.12
// packet

// usally include NKCore.h but it is header only
#include "../NKCore/ByteStream.h"
#include "../NKCore/Buffer.h"
#include "RecvStream.h"
#include "SendStream.h"
#include "Protocol.h"

namespace NKNetwork
{
	class Packet
	{
		// for read by external library
	public:
		inline NKCore::byte *getData(void) const { if (getDataSize() == 0) { return nullptr; } else { return _pProtocolData; } }

		inline PROTOCOLHEAD::size_type getSize(void) const { if (_pHeader != nullptr) { return _pHeader->_size; } return 0; }

	public:
		inline PROTOCOLHEAD::size_type getDataSize(void) const { if (_pHeader != nullptr) { return _pHeader->_size - sizeof(PROTOCOLHEAD); } return 0; }
		inline uint16_t getCommandID(void) const { if (_pHeader != nullptr) { return _pHeader->_commandid; } return 0; }
				
	protected:
		// @nolimitk 구조체 접근을 위한 interface
		PROTOCOLHEAD *_pHeader;
		NKCore::byte *_pProtocolData;

		// copy pakcet for asynchronous processing
	protected:
		NKCore::ByteStream<NKCore::Buffer> _copy_stream;

	public:
		Packet(const Packet& param)
			: _pHeader(nullptr)
			, _pProtocolData(nullptr)
			, _copy_stream(param.getSize())
		{
			// copy stream
			_copy_stream.write(reinterpret_cast<NKCore::byte*>(param._pHeader), sizeof(PROTOCOLHEAD));
			if (param.getDataSize() > 0)
			{
				_copy_stream.write(param.getData(), param.getDataSize());
			}

			// set pointers
			if (_copy_stream.read((NKCore::byte**)&_pHeader, sizeof(PROTOCOLHEAD)) == false) return;
			if (getDataSize() > 0)
			{
				if (_copy_stream.read(&_pProtocolData, getDataSize()) == false) return;
			}
		}

		Packet(RecvStream& stream)
			: _pHeader(nullptr)
			, _pProtocolData(nullptr)
			, _copy_stream(0)
		{
			// @TODO exception...
			if (stream.read((NKCore::byte**)&_pHeader, sizeof(PROTOCOLHEAD)) == false) return;
			if (getDataSize() > 0)
			{
				if (stream.read(&_pProtocolData, getDataSize()) == false) return;
			}
		}

		Packet(const SendStream& stream)
			: _pHeader(nullptr)
			, _pProtocolData(nullptr)
			, _copy_stream(0)
		{
			_pHeader = reinterpret_cast<PROTOCOLHEAD*>(stream.get());
			if (_pHeader->_size > sizeof(PROTOCOLHEAD) && stream.getLength() == _pHeader->_size)
			{
				_pProtocolData = (stream.get() + sizeof(PROTOCOLHEAD));
			}
		}
		//
				
	public:
		Packet(void)
			: _pHeader(nullptr)
			, _pProtocolData(nullptr)
			, _copy_stream(0)
		{
		}
		virtual ~Packet(void)
		{
		}
	};
}

#endif // __PACKET_HEADER__