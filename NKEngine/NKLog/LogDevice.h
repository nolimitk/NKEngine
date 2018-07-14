#pragma once
#ifndef __LOGDEVICE_HEADER__
#define __LOGDEVICE_HEADER__
// @nolimitk
// 16.04.05
// log deivce

#include "../NKCore.h"

namespace NKLog
{
	class LogLayout;
	class LogBuilder;
	class LogCategory;

	class LogDevice
	{
		// @thread-safe, @lock
	protected:
		std::mutex _mutex;

	public:
		bool writeSafe(const LogLayout& layout, const LogCategory& category, const NKWString& log);
		///

		// @not-thread-safe
	public:
		bool write(const LogLayout& layout, const LogCategory& category, const NKWString& log);
		virtual bool onWrite(const LogLayout& layout, const LogCategory& category, const NKWString& log) = 0;
		/// @not-safe

	public:
		static bool replaceInvalid(wchar_t *pBuffer);
		
		// @thread-safe
		// log category
	public:
		bool registerCategory(const NKCore::UniqueID& id) { return _category_set.registerID(id); }
		bool unregisterCategory(const NKCore::UniqueID& id) { return _category_set.unregisterID(id); }
		bool isRegisteredCategory(const NKCore::UniqueID& id) const { return _category_set.isRegisteredID(id); }

	protected:
		NKCore::UniqueIDSetSafe _category_set;
		///

		// @thread-safe
		// log layout
	public:
		bool registerLayout(const NKCore::UniqueID& id) { return _layout_set.registerID(id); }
		bool unregisterLayout(const NKCore::UniqueID& id) { return _layout_set.unregisterID(id); }
		bool isRegisteredLayout(const NKCore::UniqueID& id) const { return _layout_set.isRegisteredID(id); }

	protected:
		NKCore::UniqueIDSetSafe _layout_set;
		///

		// @not-thread-safe
		// log builder
	public:
		bool setBuilder(std::unique_ptr<LogBuilder>&& builder)
		{
			_builder = std::move(builder);
			return true;
		}

	protected:
		std::unique_ptr<LogBuilder> _builder;
		///

	public:
		LogDevice(void);
		virtual ~LogDevice(void);
	};
}

#endif // __LOGDEVICE_HEADER__