#include "LogBuilder.h"

#include <WinSock2.h>
#include <Windows.h>
#include <sstream>

#include "LogCategory.h"
#include "LogLayout.h"

using namespace std;
using namespace NKLog;

NKWString BaseLogBuilder::publish(const LogLayout& layout, const LogCategory& category, const NKWString& log)
{
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);

	wchar_t strTime[128];
	swprintf(strTime, sizeof(strTime), L"%02d/%02d/%04d %02d:%02d:%02d.%03d,",
		systemTime.wMonth, systemTime.wDay, systemTime.wYear,
		systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);

	wostringstream build_log;
	build_log << strTime << L"[" << layout._name << L"]," << L"[" << category._name << L"]," << log;

	return build_log.str();
}