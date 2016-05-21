#pragma once
#ifndef __NKDATATYPES_HEADER__
#define __NKDATATYPES_HEADER__
// @nolimitk
// 16.05.10
// data types

#include <WinSock2.h>
#include <Windows.h>

// @todo it will be better to removed
#pragma warning(disable: 4100) // unreferenced formal parameter
//#pragma warning(disable: 4189) // local variable is initialized but not referenced
//#pragma warning(disable: 4244) // conversion from XX to YY, possible loss of data
//
//// 없애기 애매한 워닝들.
//#pragma warning(disable: 4201) // nonstandard extension used : nameless struct/union
#pragma warning(disable: 4239) // nonstandard extension used : 'token' : conversion from 'type' to 'type'
//#pragma warning(disable: 4127) // conditional expression is constant

typedef unsigned long		ulong;
typedef unsigned int		uint;
typedef unsigned short		ushort;
typedef char				int8;
typedef __int64				int64;
typedef unsigned __int64	uint64;
typedef wchar_t				wchar;

#endif // __NKDATATYPES_HEADER__