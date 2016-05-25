#include "ConsoleCommander.h"
#include <iostream>

using namespace NKEngine;
using namespace std;

ConsoleCommander::ConsoleCommander(void *pOwner, const StringCommander::COMMANDER *commands, int countCommands)
	:StringCommander(commands, countCommands, 0)
	,_pOwner(pOwner)
{

}

bool ConsoleCommander::onRun(void)
{
	NKWString command;

	do
	{
		getline( wcin, command );
		if (command.length() == 0)
		{
			continue;
		}

		if( parseCommandLine( _pOwner, command) == false )
		{
			break;
		}

	}while( true );

	return true;
}

bool ConsoleCommander::onExecuteCommand( void *pOwner, wchar_t *wstrCommand, wchar_t *wstrParams[MAX_PARAMETERS], wchar_t *longParam, int countParams )
{
	for( int i = 0 ; i < _countCommands ; i++ )
	{
		if( wcscmp( wstrCommand, _commands[i].wstrCommand ) == 0 )
		{
			if( _commands[i].paramType == PARAMTYPE_LONG )
			{
				// 0번째 인자를 나머지 모두로 설정한다.
				wstrParams[0] = longParam;
			}
			else if( _commands[i].paramType == PARAMTYPE_TOKEN )
			{
				if( countParams < _commands[i].countParams )
				{
					onCommandFailed( pOwner, i, wstrParams, countParams, COMMANDER_RESULTCODE_NOTMATCHED_COUNT_PARAMETERS );
					return true;
				}
			}
			else
			{
				onCommandFailed( pOwner, i, wstrParams, countParams, COMMANDER_RESULTCODE_WRONG_PARAMTYPE );
				return true;
			}

			if( _commands[i].minAuthority > _authority )
			{
				onCommandFailed( pOwner, i, wstrParams, countParams, COMMANDER_RESULTCODE_NOTHAVE_AUTHORITY );
				return true;
			}

			return _commands[i].funcCommander( pOwner, wstrParams, countParams );
		}
	}

	onCommandFailed( pOwner, -1, wstrParams, countParams, COMMANDER_RESULTCODE_UNKNOWN_COMMAND );

	return true;
}