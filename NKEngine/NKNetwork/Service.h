#pragma once
#ifndef __SERVICE_HEADER__
#define __SERVICE_HEADER__
// @nolimitk
// 18.07.14
// service

#include <unordered_map>
#include <memory>
#include <mutex>
#include "../NKCore.h"
#include "NetworkCallbacks.h"
#include "NKNetworkConstants.h"

namespace NKNetwork
{
	class AsyncServerSocket;
	class ServiceServerCallback;
	class ServiceClientCallback;

	class Service
	{
	public:
		bool start(void);
		bool close(void);
		bool registerServerCallback(const std::shared_ptr<ServerCallback>& callback);

	protected:
		const uint16_t _port;
		std::shared_ptr<AsyncServerSocket> _server_socket;

		/// connection map
	protected:
		using ConnectionMap = std::unordered_map<ConnectionID, ConnectionSP>;
		
	public:
		bool insertConnection(const ConnectionSP& connection);
		bool eraseConnection(const ConnectionID& id);
		size_t getConnectionCount(void) const;

	protected:
		mutable std::mutex __mutex_connection_map;
		ConnectionMap _connection_map;
		///

	public:
		Service(uint16_t port);
	};

	class ServiceServerCallback : public ServerCallback
	{
		void onAccepted(ConnectionSP& connection) override;
		void onClosed(void) override {}

	protected:
		Service & _service;

	public:
		ServiceServerCallback(Service& service);
	};

	class ServiceClientCallback : public ClientCallback
	{
		void onConnected(const ConnectionSP& connection) override {}
		void onConnectFailed(const ConnectionSP& connection) override {}
		void onReceived(const ConnectionSP& connection, const Packet& packet) override {}
		void onSent(const ConnectionSP& connection) override {}

		void onClosed(const ConnectionSP& connection) override;

	protected:
		Service & _service;

	public:
		ServiceClientCallback(Service& service);
	};	
}

#endif // __SERVICE_HEADER__