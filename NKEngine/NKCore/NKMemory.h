#pragma once
#ifndef __NKMEMORY_HEADER__
#define __NKMEMORY_HEADER__
// @nolimitk
// 16.04.02
// memory

#include "../../external/vld/inc/vld.h"

void* operator new(size_t size);
void operator delete(void *p);

#define SAFE_DELETE(p) { if(p){ delete p; p=nullptr; } }

#endif // __NKMEMORY_HEADER__