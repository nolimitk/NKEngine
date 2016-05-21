#pragma once
#ifndef __LOG_HEADER__
#define __LOG_HEADER__
// @nolimitk
// 16.04.05
// log

#include "../NKCore.h"
#include <vector>

namespace NKLog
{
	class LogDevice;
	class LogLayout;
	class LogCategory;
	class LogBuilder;

	class Log
	{
		// @not-thread-safe
	public:
		bool registerLogDevice(std::unique_ptr<LogDevice>&& logDevice);
		bool write(const LogLayout& layout, const LogCategory& category, const NKWString& log);
		void clearLogDevices(void);

	protected:
		bool writeDeviceDetails(const LogLayout& layout, const LogCategory& category, const NKWString& log);

	protected:
		std::vector<std::unique_ptr<LogDevice>> _logDeviceQueue;
		/// @not-thread-safe

		// @thread-safe, @wait-free
	public:
		inline void turnOn(void) { _turnon = true; }
		inline void turnOff(void) { _turnon = false; }
		inline bool getTurnOn(void) { return _turnon; }

	protected:
		volatile bool _turnon;
		// @thread-safe, @wait-free

		// @not-thread-safe
		// log builder
	public:
		//std::unique_ptr<LogBuilder> getbuilder(void) { return _builder; }
		bool setBuilder(std::unique_ptr<LogBuilder>&& builder)
		{
			_builder = std::move(builder);
			return true;
		}

	protected:
		std::unique_ptr<LogBuilder> _builder;
		///
		
	public:
		Log(void);
		virtual ~Log(void);

		friend class LogThread;
	};
}

#endif // __LOG_HEADER__