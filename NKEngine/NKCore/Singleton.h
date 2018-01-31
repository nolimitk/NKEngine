#pragma once
#ifndef __SINGLETON_HEADER__
#define __SINGLETON_HEADER__
// @nolimitk
// 16.05.10
// singleton as template

#include <mutex>

namespace NKCore
{
	template<class T>
	class Singleton
	{
	public:
		static T* getInstance(void)
		{
			if (_singleton == nullptr)
			{
				std::lock_guard<std::mutex> _lock(_mutex);
				{
					if (_purge_singleton == false && _singleton == nullptr)
					{
						_singleton = new T;
					}
				}
			}

			return _singleton;
		}

		static void destroy(void)
		{
			SAFE_DELETE(_singleton);
		}

	protected:
		static T* _singleton;
		static std::mutex _mutex;
		static bool _purge_singleton;

	public:
		Singleton(void) {}
		virtual ~Singleton(void) {}
	};

	template<class T> T* Singleton<T>::_singleton = nullptr;
	template<class T> std::mutex Singleton<T>::_mutex;
	template<class T> bool Singleton<T>::_purge_singleton = false;
}

#endif // __SINGLETON_HEADER__