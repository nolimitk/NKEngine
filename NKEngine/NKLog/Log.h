#pragma once
#ifndef __LOG_HEADER__
#define __LOG_HEADER__
// @nolimitk
// 16.04.05
// log

#include "../NKCore.h"
#include <list>

namespace NKLog
{
	class LogDevice;
	class LogLayout;
	class LogCategory;
	class LogBuilder;

	class Log
	{
		// @thread-safe
	public:
		bool registerLogDevice(const std::shared_ptr<LogDevice>& logDevice);
		bool write(const LogLayout& layout, const LogCategory& category, const NKWString& log);
		void clearLogDevices(void);

	protected:
		bool emptyDeviceList(void) const { std::lock_guard<std::mutex> _lock(__mutex_logDeviceList); return _logDeviceList.empty(); }
		bool writeDeviceDetails(const LogLayout& layout, const LogCategory& category, const NKWString& log);

	protected:
		std::list<std::shared_ptr<LogDevice>> _logDeviceList;
		mutable std::mutex __mutex_logDeviceList;
		/// @thread-safe

		// @thread-safe, @wait-free
	public:
		inline void turnOn(void) { _turnon = true; }
		inline void turnOff(void) { _turnon = false; }
		inline bool getTurnOn(void) const { return _turnon; }

	protected:
		volatile bool _turnon;
		// @thread-safe, @wait-free
				
	public:
		Log(void);
		virtual ~Log(void);
	};
}

#endif // __LOG_HEADER__