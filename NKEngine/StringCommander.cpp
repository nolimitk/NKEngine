#include "StringCommander.h"
#include <algorithm>
#include <string>
#include <stdlib.h>

using namespace NKEngine;
using namespace std;

StringCommander::StringCommander(const StringCommander::COMMANDER *commands, int countCommands, int authority )
	:_commands(commands)
	,_countCommands(countCommands)
	,_authority(authority)
{
}

StringCommander::~StringCommander(void)
{
}

bool StringCommander::parseCommandLine(void *pOwner, NKWString& builder_command)
{
	transform(builder_command.begin(), builder_command.end(), builder_command.begin(), tolower );
		
	wchar_t *command = nullptr;
	wchar_t *params[MAX_PARAMETERS] = {NULL,};
	int countParams = 0;

	wchar_t *token = nullptr;
	wchar_t *nextToken = nullptr;
	wchar_t longParam[MAX_LENGTH_LONGPARAMETER] = {NULL, };

	token = wcstok_s( (wchar_t *)builder_command.c_str(), L" ", &nextToken );
	command = token;
	wcscpy_s(longParam, nextToken);
	while( token != NULL )
	{
		token = wcstok_s( NULL, L" ", &nextToken );
		if( token != NULL )
		{
			params[countParams++] = token;
		}
	}

	return onExecuteCommand( pOwner, command, params, longParam, countParams );
}

bool StringCommander::onExecuteCommand( void *pOwner, wchar_t *wstrCommand, wchar_t *wstrParams[MAX_PARAMETERS], wchar_t *longParam, int countParams )
{
	return true;
}