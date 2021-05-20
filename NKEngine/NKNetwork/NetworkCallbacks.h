#pragma once
#ifndef __NETWORKCALLBACKS_HEADER__
#define __NETWORKCALLBACKS_HEADER__
// @nolimitk
// 18.06.03
// network callbacks

#include <memory>
#include "NetworkConstants.h"

namespace NKNetwork
{
	// interface
	class ServerCallback
	{
	public:
		virtual void onAccepted(ConnectionSP& connection) = 0;
		virtual void onClosed(void) = 0;
	};

	class DefaultServerCallback : ServerCallback
	{
	public:
		void onAccepted(ConnectionSP& connection) override {}
		void onClosed(void) override {}
	};

	class Packet;

	// interface
	class ClientCallback
	{
	public:
		virtual void onConnected(const ConnectionSP& connection) = 0;
		virtual void onReceived(const ConnectionSP& connection, const Packet& packet) = 0;
		virtual void onSent(const ConnectionSP& connection) = 0;
		virtual void onConnectFailed(const ConnectionSP& connection) = 0;
		virtual void onClosed(const ConnectionSP& connection) = 0;
	};

	class DefaultClientCallback : public ClientCallback
	{
	public:
		void onConnected(const ConnectionSP& connection) override {}
		void onReceived(const ConnectionSP& connection, const Packet& packet) override {}
		void onSent(const ConnectionSP& connection) override {}
		void onConnectFailed(const ConnectionSP& connection) override {}
		void onClosed(const ConnectionSP& connection) override {}
	};

	class WorkerCallback
	{
	public:
		virtual void onUpdate(int64_t delta) = 0;
	};
}

#endif // __NETWORKCALLBACKS_HEADER__