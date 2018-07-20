#pragma once
#ifndef __NETWORK_INTERFACE_HEADER__
#define __NETWORK_INTERFACE_HEADER__
// @nolimitk
// 18.07.21
// network interface

#include "IOCPManager.h"
#include "AsyncSocket.h"

namespace NKNetwork
{
	template<class CallBack>
	inline std::shared_ptr<AsyncSocket> createSocket(void)
	{
		SOCKET socket_handle = IOCPManager::getInstance()->openSocket();
		if (socket_handle == INVALID_SOCKET)
		{
			return false;
		}

		std::shared_ptr<CallBack> callback = std::make_shared<CallBack>();
		return std::make_shared<AsyncSocket>(socket_handle, callback);
	}
}

#endif // __NETWORK_INTERFACE_HEADER__