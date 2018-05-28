#pragma once
#ifndef __LOGDEVICEDETAILS_HEADER__
#define __LOGDEVICEDETAILS_HEADER__
// @nolimitk
// 16.04.15
// log deivce details : console, file

#include <fstream>
#include <iostream>
#include "LogDevice.h"

namespace NKLog
{
	void errorHandling(const NKWString& name, std::wostream& out, const NKWString& log);

	class ConsoleLog : public LogDevice
	{
	public:
		virtual bool onWrite(const LogLayout& layout, const LogCategory& category, const NKWString& log) override;

	public:
		ConsoleLog(void);
		virtual ~ConsoleLog(void);
	};

	class FileLog : public LogDevice
	{
	public:
		virtual bool onWrite(const LogLayout& layout, const LogCategory& category, const NKWString& log) override;

	protected:
		void close(void);
		
	protected:
		std::wofstream _fpLog;

		// @not-thread-safe
	public:
		FileLog(void) {}
		FileLog(const NKWString& filename);
		virtual ~FileLog(void);
		/// @not-thread-safe
	};
		
	class DailyFileLog : public FileLog
	{
	public:
		virtual bool onWrite(const LogLayout& layout, const LogCategory& category, const NKWString& log) override;

		// @not-thread-safe
	protected:
		bool create(const NKWString& rootname, const NKWString& filename);
		/// @not-thread-safe

	protected:
		const NKWString _filename;
		const NKWString _root_directory;
		struct tm _last_creation_time;

		// @not-thread-safe
	public:
		DailyFileLog(const NKWString& filename);
		DailyFileLog(const NKWString& rootname, const NKWString& filename);
		virtual ~DailyFileLog(void);
		/// @not-thread-safe
	};
}

#endif // __LOGDEVICEDETAILS_HEADER__