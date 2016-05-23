#pragma once
#ifndef __NETWORKEVENT_HEADER__
#define __NETWORKEVENT_HEADER__
// @nolimitk
// 14.01.14
// network event

namespace NKNetwork
{
	class Packet;
	class Connection;

	// interface
	class NetworkEvent
	{
	public:
		virtual void onClosed(Connection& connection) = 0;
		virtual void onReceived(Connection& connection, const Packet& packet) = 0;
		virtual void onSent(Connection& connection) = 0;
	};

	// interface
	class ClientNetworkEvent : public NetworkEvent
	{
	public:
		virtual void onConnected(Connection& connection) = 0;
		// if it returns false, it will be suspended to reconnect [2014/11/24/ nolimitk]
		virtual bool onReconnect(Connection& connection) { return true; }
		virtual void onConnectFailed(Connection& connection) = 0;
	};

	// interface
	class ServerNetworkEvent : public NetworkEvent
	{
	public:
		virtual void onAccepted(Connection& connection) = 0;
	};
}

#endif // __NETWORKEVENT_HEADER__