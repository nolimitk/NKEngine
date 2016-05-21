#include "NKMemory.h"
#if defined __TURNOFF_MEMORYPOOL__
#include <stdlib.h>
#else
#include "../../external/tbb/inc/scalable_allocator.h"
#endif

void* operator new(size_t size)
{
#if defined __TURNOFF_MEMORYPOOL__
	return malloc(size);
#else
	return scalable_malloc(size);
#endif
}

void operator delete(void *p)
{
#if defined __TURNOFF_MEMORYPOOL__
	return free(p);
#else
	scalable_free(p);
#endif
}