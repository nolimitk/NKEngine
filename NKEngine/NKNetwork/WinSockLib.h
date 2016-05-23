#ifndef __WINSOCKLIB_HEADER__
#define __WINSOCKLIB_HEADER__
// @nolimitk
// 13.08.26
// simple wrapping class

#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

namespace NKNetwork
{
	class WinSockLib
	{
	public:
		// return :: if successful, returns zero
		// return value of WSAStartup
		int open(void);
		// return :: if successful, returns zero
		// return value of WSACleanup
		int close(void);

	protected:
		WSAData _wsaData;

	public:
		WinSockLib(void);
		~WinSockLib(void);
	};
}

#endif // __WINSOCKLIB_HEADER__

