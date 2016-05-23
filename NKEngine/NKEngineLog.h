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
		
public:
	NKEngineLog(void);
	virtual ~NKEngineLog(void);
};

using NKEngineLogSingleton = NKCore::Singleton<NKEngineLog>;

#if defined _DEBUG
#define NKENGINELOG_ERROR(fmt,...)				NKEngineLogSingleton::getInstance()->write(NKEngineLog::LAYOUT::LAYOUT_ERROR, __FUNCTIONW__, __LINE__, fmt, __VA_ARGS__);
#define NKENGINELOG_WARNING(fmt,...)			NKEngineLogSingleton::getInstance()->write(NKEngineLog::LAYOUT::LAYOUT_WARNING, __FUNCTIONW__, __LINE__, fmt, __VA_ARGS__);
#define NKENGINELOG_INFO(fmt,...)				NKEngineLogSingleton::getInstance()->write(NKEngineLog::LAYOUT::LAYOUT_INFO, __FUNCTIONW__, __LINE__, fmt, __VA_ARGS__);
#define NKENGINELOG_SOCKETERROR(err,fmt,...)	NKEngineLogSingleton::getInstance()->writeSocketError(__FUNCTIONW__, __LINE__, err, fmt, __VA_ARGS__);
#else
#define NKENGINELOG_ERROR(fmt,...)				NKEngineLogSingleton::getInstance()->write(NKEngineLog::LAYOUT::LAYOUT_ERROR, __FILEW__, __LINE__, fmt, __VA_ARGS__);
#define NKENGINELOG_WARNING(fmt,...)			NKEngineLogSingleton::getInstance()->write(NKEngineLog::LAYOUT::LAYOUT_WARNING, __FILEW__, __LINE__, fmt, __VA_ARGS__);
#define NKENGINELOG_INFO(fmt,...)				NKEngineLogSingleton::getInstance()->write(NKEngineLog::LAYOUT::LAYOUT_INFO, __FILEW__, __LINE__, fmt, __VA_ARGS__);
#define NKENGINELOG_SOCKETERROR(err,fmt,...)	NKEngineLogSingleton::getInstance()->writeSocketError(__FILEW__, __LINE__, err, fmt, __VA_ARGS__);
#endif

#endif // __NKENGINELOG_HEADER__