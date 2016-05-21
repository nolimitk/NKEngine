#pragma once
#ifndef __BUFFER_HEADER__
#define __BUFFER_HEADER__
// @nolimitk
// 16.05.01
// buffer

#include <memory>
#include "NKDataTypes.h"

namespace NKCore
{
	class Buffer
	{
	protected:
		byte* get(void) const { return _internal.get(); }

	protected:
		std::unique_ptr<byte[]> _internal;
		const size_t _size;
				
	public:
		Buffer(size_t size)
			:_size(size)
		{
			if (size > 0)
			{
				_internal = std::make_unique<byte[]>(size);
			}
		}

		friend class ByteStream;
		friend class ReadStream;
		friend class WriteStream;
	};
}

#endif // __BUFFER_HEADER__