#pragma once
#ifndef __NETWORKCALLBACKS_HEADER__
#define __NETWORKCALLBACKS_HEADER__
// @nolimitk
// 18.06.03
// network callbacks

#include <memory>

namespace NKNetwork
{
	class AsyncSocket;

	// interface
	class ServerCallback
	{
	public:
		virtual void onAccepted(std::shared_ptr<AsyncSocket> socket) = 0;
		virtual void onClosed(void) = 0;
	};

	class DefaultServerCallback : ServerCallback
	{
	public:
		void onAccepted(std::shared_ptr<AsyncSocket> socket) override {}
		void onClosed(void) override {}
	};

	class Packet;

	// interface
	class ClientCallback
	{
	public:
		virtual void onConnected(std::shared_ptr<AsyncSocket> socket) = 0;
		virtual void onReceived(std::shared_ptr<AsyncSocket> socket, const Packet& packet) = 0;
		virtual void onSent(std::shared_ptr<AsyncSocket> socket) = 0;
		virtual void onConnectFailed(std::shared_ptr<AsyncSocket> socket) = 0;
		virtual void onClosed(std::shared_ptr<AsyncSocket> socket) = 0;
	};

	class DefaultClientCallback : public ClientCallback
	{
	public:
		void onConnected(std::shared_ptr<AsyncSocket> socket) override {}
		void onReceived(std::shared_ptr<AsyncSocket> socket, const Packet& packet) override {}
		void onSent(std::shared_ptr<AsyncSocket> socket) override {}
		void onConnectFailed(std::shared_ptr<AsyncSocket> socket) override {}
		void onClosed(std::shared_ptr<AsyncSocket> socket) override {}
	};

	class ClientNetworkEvent
	{
	public:
		virtual void onConnected(AsyncSocket& connection) = 0;
		virtual void onReceived(AsyncSocket& connection, const Packet& packet) = 0;
		virtual void onSent(AsyncSocket& connection) = 0;
		// if it returns false, it will be suspended to reconnect [2014/11/24/ nolimitk]
		virtual bool onReconnect(AsyncSocket& connection) { return true; }
		virtual void onConnectFailed(AsyncSocket& connection) = 0;
	};
}

#endif // __NETWORKCALLBACKS_HEADER__