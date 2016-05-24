#pragma once
#ifndef __NKNETWORK_HEADER__
#define __NKNETWORK_HEADER__
// @nolimitk
// 16.05.02
// nk network

#include "NKNetwork/WinSockLib.h"
#include "NKNetwork/IOCPManager.h"
#include "NKNetwork/Protocol.h"
#include "NKNetwork/Event.h"
#include "NKNetwork/EventContext.h"
#include "NKNetwork/Packet.h"
#include "NKNetwork/RecvStream.h"
#include "NKNetwork/AsyncSocket.h"
#include "NKNetwork/AsyncServerSocket.h"
#include "NKNetwork/Connection.h"

#pragma comment(lib, "Mswsock.lib")

#endif // __NKNETWORK_HEADER__
