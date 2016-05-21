#pragma once
#ifndef __NKENGINE_HEADER__
#define __NKENGINE_HEADER__
// @nolimitk
// 16.05.15
// nk engine

#if defined _DEBUG
#define __TURNOFF_MEMORYPOOL__
#endif

#include "NKCore.h"
#include "NKLog.h"

#if defined _DEBUG
#pragma comment(lib, "nkengine_debug.lib")
#else
#pragma comment(lib, "nkengine.lib")
#endif

#endif // __NKENGINE_HEADER__

