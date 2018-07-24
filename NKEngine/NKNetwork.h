#pragma once
#ifndef __NKNETWORK_HEADER__
#define __NKNETWORK_HEADER__
// @nolimitk
// 16.05.02
// nk network

#include "NKNetwork/IOCPManager.h"
#include "NKNetwork/EventContext.h"
#include "NKNetwork/Packet.h"
#include "NKNetwork/RecvStream.h"
#include "NKNetwork/SendStream.h"
#include "NKNetwork/Protocol.h"
#include "NKNetwork/AsyncSocket.h"
#include "NKNetwork/AsyncServerSocket.h"
#include "NKNetwork/NetworkConstants.h"
#include "NKNetwork/NetworkInterface.h"
#include "NKNetwork/NetworkCallbacks.h"
#include "NKNetwork/Service.h"

#pragma comment(lib, "Mswsock.lib")

#endif // __NKNETWORK_HEADER__
