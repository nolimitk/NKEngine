#pragma once
#ifndef __ASYNCLOG_HEADER__
#define __ASYNCLOG_HEADER__
// @nolimitk
// 13.09.13
// asynchronous log

#include <string>
#include <atomic>

#include "../NKCore.h"
#include "LogLayout.h"
#include "LogCategory.h"
#include "Log.h"

namespace NKLog
{
	class LogData : public NKCore::TNode2<LogData>
	{
	public:
		const LogLayout _log_layout;
		const LogCategory _log_category;
		const NKWString _log;

	public:
		LogData(const LogLayout& layout, const LogCategory& category, const NKWString& log)
			:_log_layout(layout)
			, _log_category(category)
			, _log(log)
		{
		}
		virtual ~LogData(void) {}
	};

	using LogDataSP = std::shared_ptr<LogData>;

	// log thread must be independent system.
	class LogThread : public NKCore::NKThread
	{
	public:
		virtual bool onRun(void) override;

	protected:
		static const std::chrono::milliseconds DEFAULT_GAP_LOGWRITE;

	public:
		LogThread(void)
		{
		}
		virtual ~LogThread(void) {}
	};
		
	class AsyncLog : public Log
	{
		// @thread-safe, @lock-free
	public:
		bool write(const LogLayout& layout, const LogCategory& category, const NKWString& log);
		///
		
	protected:
		LogDataSP popLogdataQueue(void);
		bool write(const LogDataSP& pLogData);
		bool flush(void);

	protected:
		NKCore::TWaitFreeQueue<LogData> _logdata_queue;

		LogThread _log_thread;
		std::atomic<bool> _start_thread;
		
	public:
		AsyncLog(void);
		virtual ~AsyncLog(void);

		friend class LogThread;
	};

	using AsyncLogSingleton = NKCore::Singleton<AsyncLog>;
}

#endif // __ASYNCLOG_HEADER__