#include "NKEngineLog.h"

#include <stdarg.h>
#include <WinSock2.h>
#include <Windows.h>
#include <sstream>

using namespace NKLog;
using namespace std;

NKEngineLog::NKEngineLog(void)
	:NKLogger(LogCategory(L"NKENGINE"))
{
	ConsoleLoggerSingleton::getInstance()->get()->registerCategory(_category._id);
}

NKEngineLog::~NKEngineLog(void)
{

}

bool NKEngineLog::writeSocketError(const wchar_t* file, int line, uint32_t err, const wchar_t* format, ...)
{
	if (format == nullptr) return false;
	if (file == nullptr) return false;

	LPTSTR pErrorText = NULL;

	if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pErrorText, 0, NULL) <= 0)
	{
		return false;
	}
		
	va_list argptr;
	va_start(argptr, format);

	wchar_t buffer[1024];
	vswprintf(buffer, 1024, format, argptr);

	bool ret = write(LAYOUT_ERROR, file, line, L"[SOCKETERROR],%u,%s,%s", err, pErrorText, buffer);
	
	if (pErrorText != nullptr)
	{
		LocalFree(pErrorText);
		pErrorText = nullptr;
	}
	return ret;
}

bool NKEngineLog::turnoffConsole(void)
{
	return ConsoleLoggerSingleton::getInstance()->get()->unregisterCategory(_category._id);
}
