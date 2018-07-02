#pragma once
#ifndef __TNODE_HEADER__
#define __TNODE_HEADER__
// @nolimitk
// 16.05.10
// template node

#include <cstdint>

namespace NKCore
{
	template<typename T>
	class TNode
	{
	public:
		inline void setNext(T* node) { _next = node; }
		inline T* getNext(void) const { return _next; }
		inline void setPrev(T* node) { _prev = node; }
		inline T* getPrev(void) const { return _prev; }

	public:
		inline bool operator < (T& param)
		{
			return _key < param._key;
		}

		inline bool operator > (uint64_t param)
		{
			return _key > param;
		}

	protected:
		uint64_t _key;
		T* _next;
		T* _prev;

	public:
		TNode(void)
			:TNode(0)
		{
		}

		TNode(uint64_t key)
			:_key(key)
			, _next(nullptr)
			, _prev(nullptr)
		{
		}

		virtual ~TNode(void) {}
	};
}

#endif // __TNODE_HEADER__