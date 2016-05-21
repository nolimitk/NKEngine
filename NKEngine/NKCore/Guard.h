#pragma once
#ifndef __GUARD_HEADER__
#define __GUARD_HEADER__
// @nolimitk
// 14.06.11
// guard

#include "NKDataTypes.h"

namespace NKCore
{
	const static int MAX_DEPTH = 10;
	const static int MAX_FUNCTION_NAME_LENGTH = 1024;
	extern __declspec(thread) wchar g_customCallstack[MAX_DEPTH][MAX_FUNCTION_NAME_LENGTH];
	extern __declspec(thread) int g_indexCallstack;

	class Guard
	{
	public:
		Guard(wchar *funcName);
		~Guard(void);
	};
}

#define __GUARD__		{ NKCore::Guard ___guard___(TEXT(__FUNCSIG__))
#define __UNGUARD__		}

#define ReportGuardCaller(type, fmt, ...)	\
{\
	if( g_indexCallstack > 0 && g_indexCallstack <= MAX_DEPTH )\
	{\
		NE_DEBUGLOG( L"caller, %s, " fmt, g_customCallstack[g_indexCallstack-1], __VA_ARGS__ );\
	}\
	else\
	{\
		NE_DEBUGLOG( L"there is no guard, stack index %d, " fmt, g_indexCallstack, __VA_ARGS__ );\
	}\
}

#endif // __GUARD_HEADER__
