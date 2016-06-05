#pragma once
#ifndef __NKUNITTESTLOG_HEADER__
#define __NKUNITTESTLOG_HEADER__
// @nolimitk
// 16.04.23
// unit test log

#include "NKEngine.h"

class NKUnittestLog : public NKLog::NKLogger
{
public:
	void turnOffConsoleLog(void);

public:
	NKUnittestLog(void);
	virtual ~NKUnittestLog(void);
};

using NKUnittestLogSingleton = NKCore::Singleton<NKUnittestLog>;

#define NKUNITTESTLOG_ERROR(fmt,...)				NKUnittestLogSingleton::getInstance()->write(NKUnittestLog::LAYOUT::LAYOUT_ERROR, __FILEW__, __LINE__, fmt, __VA_ARGS__);
#define NKUNITTESTLOG_WARNING(fmt,...)				NKUnittestLogSingleton::getInstance()->write(NKUnittestLog::LAYOUT::LAYOUT_WARNING, __FILEW__, __LINE__, fmt, __VA_ARGS__);
#define NKUNITTESTLOG_INFO(fmt,...)					NKUnittestLogSingleton::getInstance()->write(NKUnittestLog::LAYOUT::LAYOUT_INFO, __FILEW__, __LINE__, fmt, __VA_ARGS__);

#endif // __NKUNITTESTLOG_HEADER__