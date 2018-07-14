#pragma once
#ifndef __NKLOGGER_HEADER__
#define __NKLOGGER_HEADER__
// @nolimitk
// 16.04.23
// logger

#include "LogLayout.h"
#include "LogCategory.h"

namespace NKLog
{
	class NKLogger
	{
	public:
		enum LAYOUT
		{
			LAYOUT_ERROR,
			LAYOUT_WARNING,
			LAYOUT_INFO,
			LAYOUT_MAX
		};

	public:
		bool write(LAYOUT layout, const wchar_t* file, int line, const wchar_t* format, ...);

	public:
		static const NKLog::LogLayout _layout[LAYOUT_MAX];

	protected:
		bool write(LAYOUT layout, const wchar_t* format, ...);
		
		// @TODO it must be inifite
	protected:
		static const int MAX_LENGTH_LOG_BUFFER = 1024;

	protected:
		const NKLog::LogCategory _category;
				
	public:
		NKLogger(const LogCategory& category);
		virtual ~NKLogger(void);
	};
	
	class ConsoleLog;

	class ConsoleLogger
	{
	public:
		std::shared_ptr<ConsoleLog> get(void) const { return _console_log; }

	protected:
		std::shared_ptr<ConsoleLog> _console_log;

	public:
		ConsoleLogger(void);
		virtual ~ConsoleLogger(void) {}
	};
}

#endif // __NKLOGGER_HEADER__