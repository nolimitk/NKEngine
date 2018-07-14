#include "NKEngineUnitTest.h"
#include "NKUnitTest.h"
#include "NKEngineLog.h"

#include <iostream>

int main()
{
	NKEngine::StartEngine();

	NKEngineLogSingleton::getInstance()->turnoffConsoleLogInfo();

	//NKUnitTest::getInstance().run(true);
	NKUnitTest::getInstance().run(false);

	NKEngine::ReleaseEngine();

	char pause;
	std::cin >> pause;

    return 0;
}

