#pragma once
#ifndef __BUFFER_HEADER__
#define __BUFFER_HEADER__
// @nolimitk
// 16.05.01
// buffer

#include <memory>

namespace NKCore
{
	using byte = unsigned char;

	class Buffer
	{
	public:
		byte* get(void) const { return _internal.get(); }
		const size_t size(void) const { return _size; }

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
	};
}

#endif // __BUFFER_HEADER__