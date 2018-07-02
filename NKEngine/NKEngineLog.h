#pragma once
#ifndef __NKENGINELOG_HEADER__
#define __NKENGINELOG_HEADER__
// @nolimitk
// 16.04.16
// engine log

#include "NKLog.h"

class NKEngineLog : public NKLog::NKLogger
{
public:
	bool writeSocketError(const wchar_t* file, int line, uint32_t err, const wchar_t* format, ...);
	bool turnoffConsole(void);
			
public:
	NKEngineLog(void);
	virtual ~NKEngineLog(void);
};

using NKEngineLogSingleton = NKCore::Singleton<NKEngineLog>;

#define NKENGINELOG_ERROR(fmt,...)				NKEngineLogSingleton::getInstance()->write(NKEngineLog::LAYOUT_ERROR, __FUNCTIONW__, __LINE__, fmt, __VA_ARGS__);
#define NKENGINELOG_WARNING(fmt,...)			NKEngineLogSingleton::getInstance()->write(NKEngineLog::LAYOUT_WARNING, __FUNCTIONW__, __LINE__, fmt, __VA_ARGS__);
#define NKENGINELOG_INFO(fmt,...)				NKEngineLogSingleton::getInstance()->write(NKEngineLog::LAYOUT_INFO, __FUNCTIONW__, __LINE__, fmt, __VA_ARGS__);
#define NKENGINELOG_SOCKETERROR(err,fmt,...)	NKEngineLogSingleton::getInstance()->writeSocketError(__FUNCTIONW__, __LINE__, err, fmt, __VA_ARGS__);

#define NKENGINELOG_ERROR_ASSERT(fmt,...)				NKEngineLogSingleton::getInstance()->write(NKEngineLog::LAYOUT_ERROR, __FUNCTIONW__, __LINE__, fmt, __VA_ARGS__); _ASSERT(false);
#define NKENGINELOG_SOCKETERROR_ASSERT(err,fmt,...) NKEngineLogSingleton::getInstance()->writeSocketError(__FUNCTIONW__, __LINE__, err, fmt, __VA_ARGS__); _ASSERT(false);

#endif // __NKENGINELOG_HEADER__