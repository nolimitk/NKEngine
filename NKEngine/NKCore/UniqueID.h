#pragma once
#ifndef __UNIQUEIDGENERATOR_HEADER__
#define __UNIQUEIDGENERATOR_HEADER__
// @nolimitk
// 16.04.10
// unique id

#include <set>
#include <atomic>
#include <shared_mutex>

namespace NKCore
{
	class UniqueID
	{
	public:
		const uint64_t _id;

	public:
		UniqueID(void)
			:_id(UniqueID::generate())
		{
		}
		
		operator uint64_t() const
		{
			return _id;
		}
		
		// @thread-safe, @lock-free
	protected:
		static uint64_t generate(void);

	protected:
		static std::atomic<uint64_t> _seed;
		/// @thread-safe
	};

	class UniqueIDSetSafe
	{
	public:
		// @thread-safe, @rwlock-write
		bool registerID(const UniqueID& id)
		{
			std::lock_guard<std::shared_mutex> lock(_s_mutex);
			auto ret = _set.insert(id);
			if (ret.second == false)
			{
				return false;
			}
			return true;
		}

		// @thread-safe, @rwlock-write
		bool unregisterID(const UniqueID& id)
		{
			std::lock_guard<std::shared_mutex> lock(_s_mutex);
			auto ret = _set.erase(id);
			if (ret == 0)
			{
				return false;
			}
			return true;
		}

		// @thread-safe, @rwlock-read
		bool isRegisteredID(const UniqueID& id) const
		{
			std::shared_lock<std::shared_mutex> lock(_s_mutex);
			auto iter = _set.find(id);
			if (iter == _set.end())
			{
				return false;
			}
			return true;
		}

		// @thread-safe, @rwlock-read
		size_t size(void) const
		{
			std::shared_lock<std::shared_mutex> lock(_s_mutex);
			return _set.size();
		}

	protected:
		mutable std::shared_mutex _s_mutex;
		std::set<UniqueID> _set;
	};
}

#endif // __UNIQUEIDGENERATOR_HEADER__
