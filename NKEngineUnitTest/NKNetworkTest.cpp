#pragma once

#ifndef __NKNETWORKTEST_HEADER__
#define __NKNETWORKTEST_HEADER__
// @nolimitk
// 16.04.25
// network test

#include "NKEngine.h"
#include "NKUnittest.h"
#include "NKUnittestLog.h"

using namespace std;

NKTEST(WinSockLib_Test)
{
	NKNetwork::WinSockLib winsocklib;
	_ASSERT(winsocklib.open() == 0);
	_ASSERT(winsocklib.close() == 0);

	return true;
}

NKTEST(IOCPManager_Test)
{
	_ASSERT(NKNetwork::IOCPManager::getInstance()->create() == true);

	class TestEvent : public NKNetwork::EventObject
	{
		virtual bool onProcess(NKNetwork::EventContext& event_context, uint32_t transferred) override
		{
			NKUNITTESTLOG_INFO(L"%s", __FUNCTIONW__);
			return true;
		}
	};

	class TestEvent2 : public NKNetwork::EventObject
	{
		virtual bool onProcess(NKNetwork::EventContext& event_context, uint32_t transferred)
		{
			NKUNITTESTLOG_INFO(L"%s", __FUNCTIONW__);
			return true;
		}
	};

	shared_ptr<TestEvent> test_event = make_shared<TestEvent>();

	_ASSERT(NKNetwork::IOCPManager::getInstance()->postEvent(dynamic_pointer_cast<NKNetwork::EventObject>(test_event), 0) == true);

	NKNetwork::IOCPManager::getInstance()->close();

	return true;
}

void writeStream(NKNetwork::RecvStream& stream, const NKString& str)
{
	NKNetwork::ReceiveContext *pRecvContext = new NKNetwork::ReceiveContext(stream);
	byte* pRawBuffer = stream.getRemainBuffer();
	WSABUF* pWSABuf = pRecvContext->get();
	_ASSERT(pWSABuf->buf != nullptr);
	NKNetwork::PROTOCOLHEAD::command_type command = 0;
	NKNetwork::PROTOCOLHEAD::size_type size = (NKNetwork::PROTOCOLHEAD::size_type)(sizeof(NKNetwork::PROTOCOLHEAD) + str.length());
	memcpy(pWSABuf->buf, &size, sizeof(size));
	memcpy(pWSABuf->buf + sizeof(size), &command, sizeof(command));
	memcpy(pWSABuf->buf + sizeof(NKNetwork::PROTOCOLHEAD), str.c_str(), str.length());

	for (int i = 0; i < str.length(); ++i)
	{
		_ASSERT(pRawBuffer[i + sizeof(NKNetwork::PROTOCOLHEAD)] == str[i]);
	}

	pWSABuf->len = 7;
	SAFE_DELETE(pRecvContext);
}

void readStream(NKNetwork::RecvStream& stream)
{
	while (stream.getLength() >= sizeof(NKNetwork::PROTOCOLHEAD::size_type))
	{
		NKNetwork::PROTOCOLHEAD::size_type length = stream.peek<NKNetwork::PROTOCOLHEAD::size_type>();
		if (length <= stream.getLength())
		{
			NKNetwork::Packet packet;
			_ASSERT(packet.read(stream) == true);
		}
		else
		{
			break;
		}
	}
	_ASSERT(stream.moveRead());
}

NKTEST(RecvStream_Test)
{
	NKNetwork::RecvStream _recv_stream(make_shared<NKCore::Buffer>(8000));
	_ASSERT(_recv_stream.getRemainSize() == NKNetwork::AsyncSocket::BUFFER_LENGTH_8K);
	byte* pRawBuffer = _recv_stream.getRemainBuffer();
	_ASSERT(pRawBuffer != nullptr);

	writeStream(_recv_stream, "abc");

	{
		_ASSERT(_recv_stream.update(7) == true);
		_ASSERT(_recv_stream.getLength() >= sizeof(NKNetwork::PROTOCOLHEAD::size_type));
		readStream(_recv_stream);
	}

	writeStream(_recv_stream, "defghij");

	{
		_ASSERT(_recv_stream.update(1) == true);
		readStream(_recv_stream);
		_ASSERT(_recv_stream.update(1) == true);
		readStream(_recv_stream);
		_ASSERT(_recv_stream.update(4) == true);
		readStream(_recv_stream);
		_ASSERT(_recv_stream.update(5) == true);
		readStream(_recv_stream);
	}

	writeStream(_recv_stream, "abc");
	_ASSERT(_recv_stream.update(7) == true);
	writeStream(_recv_stream, "defghij");

	{
		_ASSERT(_recv_stream.update(11) == true);
		readStream(_recv_stream);
	}

	return true;
}

#endif // __NKNETWORKTEST_HEADER__
