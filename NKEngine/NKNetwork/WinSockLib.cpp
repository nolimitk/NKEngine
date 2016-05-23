#include "WinSockLib.h"
#include "../NKEngineLog.h"
#include "../NKCore.h"

using namespace NKNetwork;
using namespace std;

WinSockLib::WinSockLib(void)
{
	memset( &_wsaData, 0, sizeof(WSAData) );
}


WinSockLib::~WinSockLib(void)
{
}

int WinSockLib::open(void)
{
	int err;
	err = WSAStartup( MAKEWORD(2,2), &_wsaData );
	if( err == 0 )
	{
		NKENGINELOG_INFO(L"success to wsastartup,version %d,highversion %d", _wsaData.wVersion, _wsaData.wHighVersion);
		NKWString system_status = MultibyteToUnicode( _wsaData.szSystemStatus );
		NKWString description = MultibyteToUnicode( _wsaData.szDescription );
		NKENGINELOG_INFO( L"status %s,description %s", system_status.c_str(), description.c_str());
	}
	else
	{
		NKENGINELOG_ERROR( L"failed to wsastartup,%d", err );
	}
	return err;
}

int WinSockLib::close(void)
{
	int err;
	err = WSACleanup();
	if( err == 0 )
	{
		NKENGINELOG_INFO( L"success to wsacleanup" );
	}
	else
	{
		NKENGINELOG_SOCKETERROR( WSAGetLastError(), L"failed to wsacleanup" );
	}
	return err;
}