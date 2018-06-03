#include "NKLogger.h"

#include <stdarg.h>
#include <WinSock2.h>
#include <Windows.h>

#include "LogDeviceDetails.h"
#include "LogBuilder.h"
#include "AsyncLog.h"

using namespace NKLog;
using namespace std;

NKLogger::NKLogger(void)
	:NKLogger(L"NKLOG")
{
}

NKLogger::NKLogger(const NKWString& name)
	: _category(LogCategory(name))
	, _layout{ LogLayout(L"LOG_ERROR", NKLog::LogColor::RED),LogLayout(L"LOG_WARNING", NKLog::LogColor::YELLOW),LogLayout(L"LOG_INFO", NKLog::LogColor::WHITE) }
{
	std::unique_ptr<NKLog::ConsoleLog> console_log = make_unique<NKLog::ConsoleLog>();
	std::unique_ptr<NKLog::DailyFileLog> daily_file_log = make_unique<NKLog::DailyFileLog>(name);

	console_log->registerCategory(_category._id);
	daily_file_log->registerCategory(_category._id);

	for (int i = 0; i < _countof(_layout); ++i)
	{
		console_log->registerLayout(_layout[i]._id);
		daily_file_log->registerLayout(_layout[i]._id);
	}
	
	AsyncLogSingleton::getInstance()->registerLogDevice(std::move(console_log));
	AsyncLogSingleton::getInstance()->registerLogDevice(std::move(daily_file_log));
	AsyncLogSingleton::getInstance()->setBuilder(make_unique<BaseLogBuilder>());
}

NKLogger::~NKLogger(void)
{
}

bool NKLogger::write(LAYOUT layout, const wchar_t * file, int line, const wchar_t * format, ...)
{
	if (format == nullptr) return false;
	if (file == nullptr) return false;

	va_list argptr;
	va_start(argptr, format);

	wchar_t buffer[MAX_LENGTH_LOG_BUFFER];
	vswprintf(buffer, MAX_LENGTH_LOG_BUFFER, format, argptr);

	return write(layout, L"[%s],[%d],%s", file, line, buffer);
}

bool NKLog::NKLogger::write(LAYOUT layout, const wchar_t * format, ...)
{
	if (format == nullptr) return false;

	va_list argptr;
	va_start(argptr, format);

	wchar_t buffer[MAX_LENGTH_LOG_BUFFER];
	vswprintf(buffer, MAX_LENGTH_LOG_BUFFER, format, argptr);

	return AsyncLogSingleton::getInstance()->write(_layout[layout], _category, buffer);
}