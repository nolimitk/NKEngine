#include "UniqueID.h"
using namespace NKCore;

std::atomic<uint64_t> UniqueID::_seed = 0;

uint64_t UniqueID::generate(void)
{
	return ++_seed;
}
