#include "NKUtils.h"
#include <stdlib.h>

int NKEngine::randomValue( int min, int max )
{
	int amount = max - min + 1;
	if( amount <= 0 )
	{
		return min;
	}
	return (rand() % amount) + min;
}