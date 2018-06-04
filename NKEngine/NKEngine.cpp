#include "NKEngine.h"
#include "NKEngineLog.h"

bool NKEngine::StartEngine(void)
{
	return true;
}

void NKEngine::ReleaseEngine(void)
{
	NKEngineLogSingleton::destroy();
	NKLog::ConsoleLoggerSingleton::destroy();
	NKLog::AsyncLogSingleton::destroy();	
}