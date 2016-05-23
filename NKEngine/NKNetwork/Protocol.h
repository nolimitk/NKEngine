#pragma once
#ifndef __PROTOCOL_HEADER__
#define __PROTOCOL_HEADER__
// @nolimitk
// 15.07.07
// protocol

#include <stdint.h>

namespace NKNetwork
{
	struct PROTOCOLHEAD
	{
		typedef uint16_t size_type;
		typedef uint16_t command_type;

		size_type _size;
		union
		{
			struct
			{
				uint16_t _system : 1;
				uint16_t _side : 1;
				uint16_t _commandid : 14;
			};
			uint16_t _command;
		};
	};

	enum class PROTOCOLSYSTEM : uint16_t
	{
		SYSTEM,
		NORMAL
	};

	enum class PROTOCOLSIDE : uint16_t
	{
		REQ,
		ACK
	};

	enum class SYSTEM_PROTOCOL : uint16_t
	{
		NOTHING,
		PINGPONG,
	};
}

#endif // __PROTOCOL_HEADER__