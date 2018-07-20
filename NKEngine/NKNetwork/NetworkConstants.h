#pragma once
#ifndef __NETWORK_CONSTANTS_HEADER__
#define __NETWORK_CONSTANTS_HEADER__
// @nolimitk
// 18.07.15
// network constants

#include <memory>

namespace NKNetwork
{
	class AsyncSocket;
	using ConnectionSP = std::shared_ptr<AsyncSocket>;
	using ConnectionID = uint64_t;
}

#endif // __NETWORK_CONSTANTS_HEADER__