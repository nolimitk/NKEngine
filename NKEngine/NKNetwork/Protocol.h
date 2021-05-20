#pragma once
#ifndef __PROTOCOL_HEADER__
#define __PROTOCOL_HEADER__
// @nolimitk
// 15.07.07
// protocol

#include <cstdint>

namespace NKNetwork
{
	enum class PROTOCOLSYSTEM : uint16_t
	{
		SYSTEM,
		USER
	};

	enum class PROTOCOLSIDE : uint16_t
	{
		REQ,
		ACK,
		NOTI
	};

	enum class SYSTEM_PROTOCOL : uint16_t
	{
		NOTHING,
		PINGPONG,
	};

	struct PROTOCOLHEAD
	{
		typedef uint16_t size_type;
		typedef uint16_t command_type;

		size_type _size;
		
		union
		{
			struct
			{
				PROTOCOLSYSTEM _system : 1;
				PROTOCOLSIDE _side : 1;
				uint16_t _commandid : 14;
			};
			command_type _command;
		};
	};
}

#endif // __PROTOCOL_HEADER__