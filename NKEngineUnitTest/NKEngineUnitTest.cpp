#include "NKEngineUnitTest.h"
#include "NKUnitTest.h"

#include <iostream>

int main()
{
	NKUnitTest::getInstance().run(true);

	NKEngine::ReleaseEngine();

    return 0;
}

