#include "Guard.h"
#include <string.h>

using namespace NKCore;

__declspec(thread) wchar_t NKCore::g_customCallstack[MAX_DEPTH][MAX_FUNCTION_NAME_LENGTH] = {0,};
__declspec(thread) int NKCore::g_indexCallstack = 0;

Guard::Guard(wchar_t *funcName)
{
	if( g_indexCallstack < MAX_DEPTH-1 )
	{
		wcscpy_s( g_customCallstack[g_indexCallstack++], funcName );
	}
}

Guard::~Guard(void)
{
	if( g_indexCallstack > 0 )
	{
		wcscpy_s( g_customCallstack[g_indexCallstack--], L"" );
	}
}