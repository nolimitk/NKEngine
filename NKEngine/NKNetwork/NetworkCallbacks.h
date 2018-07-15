#pragma once
#ifndef __NETWORKCALLBACKS_HEADER__
#define __NETWORKCALLBACKS_HEADER__
// @nolimitk
// 18.06.03
// network callbacks

#include <memory>
#include "NKNetworkConstants.h"

namespace NKNetwork
{
	// interface
	class ServerCallback
	{
	public:
		virtual void onAccepted(const ConnectionSP& socket) = 0;
		virtual void onClosed(void) = 0;
	};

	class DefaultServerCallback : ServerCallback
	{
	public:
		void onAccepted(const ConnectionSP& socket) override {}
		void onClosed(void) override {}
	};

	class Packet;

	// interface
	class ClientCallback
	{
	public:
		virtual void onConnected(const ConnectionSP& socket) = 0;
		virtual void onReceived(const ConnectionSP& socket, const Packet& packet) = 0;
		virtual void onSent(const ConnectionSP& socket) = 0;
		virtual void onConnectFailed(const ConnectionSP& socket) = 0;
		virtual void onClosed(const ConnectionSP& socket) = 0;
	};

	class DefaultClientCallback : public ClientCallback
	{
	public:
		void onConnected(const ConnectionSP& socket) override {}
		void onReceived(const ConnectionSP& socket, const Packet& packet) override {}
		void onSent(const ConnectionSP& socket) override {}
		void onConnectFailed(const ConnectionSP& socket) override {}
		void onClosed(const ConnectionSP& socket) override {}
	};
}

#endif // __NETWORKCALLBACKS_HEADER__