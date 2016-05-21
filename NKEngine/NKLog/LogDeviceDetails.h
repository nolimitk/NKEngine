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
		// @not-thread-safe
	public:
		virtual bool create(const NKWString& filename);
		void close(void);
		/// @not-thread-safe

	public:
		virtual bool onWrite(const LogLayout& layout, const LogCategory& category, const NKWString& log) override;
		
	protected:
		std::wofstream _fpLog;

	public:
		FileLog(void);
		virtual ~FileLog(void);
	};
		
	class DailyFileLog : public FileLog
	{
		// @not-thread-safe
	public:
		void setRoot(const NKWString& root);

	public:
		virtual bool create(const NKWString& filename);
		/// @not-thread-safe

	public:
		virtual bool onWrite(const LogLayout& layout, const LogCategory& category, const NKWString& log) override;

	protected:
		NKWString _filename;
		NKWString _root_directory;
		struct tm _last_creation_time;

	public:
		DailyFileLog(void);
		virtual ~DailyFileLog(void);
	};
}

#endif // __LOGDEVICEDETAILS_HEADER__