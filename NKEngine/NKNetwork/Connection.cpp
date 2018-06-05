#include "Connection.h"
//#include "Session.h"
//#include "Service.h"
/*
#include "../NKEngineLog.h"
#include "../NKCore.h"
#include "IOCPManager.h"
#include "NetworkEvent.h"
#include "Packet.h"
#include "SendStream.h"
*/

using namespace NKNetwork;
using namespace std;

Connection::Connection(const std::shared_ptr<AsyncSocket>& socket)
	:_socket(socket)
{
}
/*
Connection::Connection(std::unique_ptr<NetworkEvent>&& network_event, bool reconnect)
	: _network_event(std::move(network_event))
	, _reconnect(reconnect)
	//,_pService(nullptr)
	//,_pSession(nullptr)
	, _heart_beat(false)
	, _last_pongtime(0)
	, _sent_ping(false)
	, _port(0)
{
}
*/

Connection::~Connection(void)
{
}

/*
bool Connection::connect(const HANDLE completion_port, const NKWString& address, uint16_t port)
{
	NKString mb_address = UnicodeToMultibyte(address);

	if (mb_address.length() == 0)
	{
		NKENGINELOG_ERROR(L"failed to connect,address is invalid string,connection %I64u, %s, %u", _id, address, port);
		return false;
	}

	return connect(completion_port, mb_address, port);
}

bool Connection::connect(const HANDLE completion_port, const NKString& address, uint16_t port)
{
	if( open(completion_port) == false )
	{
		NKENGINELOG_ERROR( L"failed to connect,to open a socket is failed,connection %I64u, %S:%d", _id, address, port);
		return false;
	}

	// for reconnecting
	_address = address;
	_port = port;

	if( AsyncSocket::connect(address, port) == false )
	{
		return false;
	}

	return true;
}
*/

/*
bool Connection::reconnect(void)
{
	if (_network_event == nullptr)
	{
		NKENGINELOG_ERROR(L"failed to reconnect,network event is null,connection %I64u, %S:%d", _id, _address, _port);
		_ASSERT(0);
		return false;
	}

	ClientNetworkEvent *pClientNetworkEvent = dynamic_cast<ClientNetworkEvent*>(_network_event.get());

	if(pClientNetworkEvent->onReconnect(*this) == false )
	{
		return false;
	}
	return connect( _address, _port);
}

bool Connection::onClosed(void)
{
	if (_network_event == nullptr)
	{
		NKENGINELOG_ERROR(L"network event is null,connection %I64u", _id);
		_ASSERT(0);
		return false;
	}

	_network_event->onClosed(*this);
	
	if( GetService() != NULL )
	{
		GetService()->DeleteConnection( GetConnectionID() );
	}

	if( _reconnect == true )
	{
		return reconnect();
	}

	return true;
}

bool Connection::onConnected(void)
{
	if (_network_event == nullptr)
	{
		NKENGINELOG_ERROR(L"network event is null,connection %I64u", _id);
		_ASSERT(0);
		return false;
	}

	ClientNetworkEvent *pClientNetworkEvent = dynamic_cast<ClientNetworkEvent*>(_network_event.get());
	pClientNetworkEvent->onConnected(*this);
	return true;
}

bool Connection::onConnectFailed(void)
{
	if (_network_event == nullptr)
	{
		NKENGINELOG_ERROR(L"network event is null,connection %I64u", _id);
		_ASSERT(0);
		return false;
	}

	ClientNetworkEvent *pClientNetworkEvent = dynamic_cast<ClientNetworkEvent*>(_network_event.get());
	pClientNetworkEvent->onConnectFailed(*this);

	if( _reconnect == true )
	{
		return reconnect();
	}

	return true;
}

bool Connection::onReceived(const Packet& packet)
{
	if (_heart_beat == true )
	{
		setPongTime();
	}

	// system packet을 처리한다.
	// @TODO : 추후 더 이쁜곳으로 옮긴다. System Network Event라던지
	if (packet.isSystem() == true)
	{
		PROTOCOLSIDE side = packet.getSide();

		if (side == PROTOCOLSIDE::ACK && (SYSTEM_PROTOCOL)(packet.getCommandID()) == SYSTEM_PROTOCOL::PINGPONG)
		{
			NKENGINELOG_INFO(L"heartbeat,connection %I64u", _id);
		}
		else if (side == PROTOCOLSIDE::REQ && (SYSTEM_PROTOCOL)(packet.getCommandID()) == SYSTEM_PROTOCOL::PINGPONG)
		{
			PROTOCOLHEAD ackHead;
			ackHead._system = (uint16_t)(PROTOCOLSYSTEM::SYSTEM);
			ackHead._side = (uint16_t)(PROTOCOLSIDE::ACK);
			ackHead._commandid = (uint16_t)(SYSTEM_PROTOCOL::PINGPONG);
			
			NKCore::Buffer buffer(BUFFER_LENGTH_8K);
			SendStream ackPacket(make_shared<NKCore::Buffer>(BUFFER_LENGTH_8K));
			ackPacket.write((byte*)&ackHead, sizeof(ackHead));

			send(ackPacket);
		}
		else
		{
			NKENGINELOG_ERROR(L"unknown system protocol, connection %I64u", _id);
			disconnect();
		}
		return true;
	}
	//

	if (_network_event == nullptr)
	{
		NKENGINELOG_ERROR(L"network event is null,connection %I64u", _id);
		_ASSERT(0);
		return false;
	}

	ClientNetworkEvent *pClientNetworkEvent = dynamic_cast<ClientNetworkEvent*>(_network_event.get());
	pClientNetworkEvent->onReceived(*this, packet);
	return true;
}

bool Connection::onSent(void)
{
	if (_network_event == nullptr)
	{
		NKENGINELOG_ERROR(L"network event is null,connection %I64u", _id);
		_ASSERT(0);
		return false;
	}

	ClientNetworkEvent *pClientNetworkEvent = dynamic_cast<ClientNetworkEvent*>(_network_event.get());
	pClientNetworkEvent->onSent(*this);
	return true;
}
*/