#include "NKEngineUnitTest.h"
#include "NKUnitTest.h"

#include <iostream>

int main()
{
	NKEngine::StartEngine();

	//NKUnitTest::getInstance().run(true);
	NKUnitTest::getInstance().run(false);

	NKEngine::ReleaseEngine();

	char pause;
	std::cin >> pause;

    return 0;
}

