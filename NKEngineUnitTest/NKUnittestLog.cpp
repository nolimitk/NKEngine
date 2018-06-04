#include "NKUnittestLog.h"

NKUnittestLog::NKUnittestLog(void)
	:NKLogger(NKLog::LogCategory(L"NKUNITTEST"))
{
	NKLog::ConsoleLoggerSingleton::getInstance()->get()->registerCategory(_category._id);
}

NKUnittestLog::~NKUnittestLog(void)
{
}