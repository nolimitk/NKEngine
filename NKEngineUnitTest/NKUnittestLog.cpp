#include "NKUnittestLog.h"

NKUnittestLog::NKUnittestLog(void)
	:NKLogger(NKLog::LogCategory(L"NKUNITTEST"))
{
	_console_logger.get()->registerCategory(_category._id);
}

NKUnittestLog::~NKUnittestLog(void)
{
}