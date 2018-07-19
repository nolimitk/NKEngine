#include "NKLogger.h"

#include <stdarg.h>
#include <WinSock2.h>
#include <Windows.h>

#include "LogDeviceDetails.h"
#include "LogBuilder.h"
#include "AsyncLog.h"

using namespace NKLog;
using namespace std;

const LogLayout NKLogger::_layout[LAYOUT_MAX]{ LogLayout(L"LOG_ERROR", NKLog::LogColor::RED), LogLayout(L"LOG_WARNING", NKLog::LogColor::YELLOW), LogLayout(L"LOG_INFO", NKLog::LogColor::WHITE) };

NKLogger::NKLogger(const LogCategory& category)
	: _category(category)
{
	std::unique_ptr<NKLog::DailyFileLog> daily_file_log = make_unique<NKLog::DailyFileLog>(category._name);
	daily_file_log->registerCategory(_category._id);

	for (const LogLayout& layout : NKLogger::_layout)
	{
		daily_file_log->registerLayout(layout._id);
	}

	daily_file_log->setBuilder(make_unique<BaseLogBuilder>());

	AsyncLogSingleton::getInstance()->registerLogDevice(std::move(daily_file_log));
}

NKLogger::~NKLogger(void)
{
}

bool NKLogger::write(LAYOUT layout, const wchar_t * file, int line, const wchar_t * format, ...)
{
	if (format == nullptr) return false;
	if (file == nullptr) return false;
	if (layout < 0 || layout >= LAYOUT_MAX) return false;

	va_list argptr;
	va_start(argptr, format);

	wchar_t buffer[MAX_LENGTH_LOG_BUFFER];
	vswprintf(buffer, MAX_LENGTH_LOG_BUFFER, format, argptr);

	return write(layout, L"[%s],[%d],%s", file, line, buffer);
}

bool NKLog::NKLogger::write(LAYOUT layout, const wchar_t * format, ...)
{
	if (format == nullptr) return false;
	if (layout < 0 || layout >= LAYOUT_MAX) return false;

	va_list argptr;
	va_start(argptr, format);

	wchar_t buffer[MAX_LENGTH_LOG_BUFFER];
	vswprintf(buffer, MAX_LENGTH_LOG_BUFFER, format, argptr);

	return AsyncLogSingleton::getInstance()->write(NKLogger::_layout[layout], _category, buffer);
}

NKLog::ConsoleLogger::ConsoleLogger(void)
{
	_console_log = make_shared<ConsoleLog>();
	
	for (const LogLayout& layout : NKLogger::_layout)
	{
		_console_log->registerLayout(layout._id);
	}
		
	_console_log->setBuilder(make_unique<BaseLogBuilder>());
	AsyncLogSingleton::getInstance()->registerLogDevice(_console_log);
}
