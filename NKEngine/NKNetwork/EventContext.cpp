#include "EventContext.h"

using namespace NKNetwork;

EventContext::EventContext(void)
	:_type(EVENTCONTEXTTYPE::NOTHING)
	, _event_object(nullptr)
{
	Internal = 0;
	InternalHigh = 0;
	hEvent = 0;
	Pointer = 0;
}

AcceptContext::AcceptContext(void)
	:EventContext()
{
	memset(_outputBuffer, 0, sizeof(_outputBuffer));
}

ConnectContext::ConnectContext(void)
	: EventContext()
{
	memset(&_server_addr, 0, sizeof(_server_addr));
}

SchedulerContext::SchedulerContext(void)
	:EventContext()
	, _param(0)
{

}

WSABUFContext::WSABUFContext(void)
	:EventContext()
{
	memset(&_wsabuf, 0, sizeof(_wsabuf));
}

NKNetwork::ReceiveContext::ReceiveContext(const NKCore::ReadStream & stream)
{
	_wsabuf.buf = (CHAR*)stream.getRemainBuffer();
	_wsabuf.len = (ULONG)stream.getRemainSize();
}

NKNetwork::SendContext::SendContext(const NKCore::WriteStream & stream)
{
	_wsabuf.buf = (CHAR*)stream.getBuffer();
	_wsabuf.len = (ULONG)stream.getLength();
}