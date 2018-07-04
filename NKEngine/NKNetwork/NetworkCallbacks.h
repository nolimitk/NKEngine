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
	class Connection;

	// interface
	class ServerCallback
	{
	public:
		virtual void onAccepted(const std::shared_ptr<Connection>& socket) = 0;
		virtual void onClosed(void) = 0;
	};

	class DefaultServerCallback : ServerCallback
	{
	public:
		void onAccepted(const std::shared_ptr<Connection>& socket) override {}
		void onClosed(void) override {}
	};

	class Packet;

	// interface
	class ClientCallback
	{
	public:
		virtual void onConnected(const std::shared_ptr<AsyncSocket>& socket) = 0;
		virtual void onReceived(const std::shared_ptr<AsyncSocket>& socket, const Packet& packet) = 0;
		virtual void onSent(const std::shared_ptr<AsyncSocket>& socket) = 0;
		virtual void onConnectFailed(const std::shared_ptr<AsyncSocket>& socket) = 0;
		virtual void onClosed(const std::shared_ptr<AsyncSocket>& socket) = 0;
	};

	class DefaultClientCallback : public ClientCallback
	{
	public:
		void onConnected(const std::shared_ptr<AsyncSocket>& socket) override {}
		void onReceived(const std::shared_ptr<AsyncSocket>& socket, const Packet& packet) override {}
		void onSent(const std::shared_ptr<AsyncSocket>& socket) override {}
		void onConnectFailed(const std::shared_ptr<AsyncSocket>& socket) override {}
		void onClosed(const std::shared_ptr<AsyncSocket>& socket) override {}
	};
}

#endif // __NETWORKCALLBACKS_HEADER__