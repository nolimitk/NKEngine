#ifndef __STRING_COMMANDER_HEADER__
#define __STRING_COMMANDER_HEADER__
// @nolimitk
// 14.09.24
// string commander

//#pragma warning(disable: 4512) // assignment operator could not be generated

#include "NKCore.h"

namespace NKEngine
{
	class StringCommander
	{
	public:
		const static int MAX_PARAMETERS = 10;
		const static int MAX_LENGTH_LONGPARAMETER = 1024;

		enum PARAMTYPE
		{
			PARAMTYPE_NOTHING,
			PARAMTYPE_TOKEN,
			PARAMTYPE_LONG
		};

		using COMMANDERFUNCTION = bool(void* pOwner, wchar_t* wstrParams[MAX_PARAMETERS], int countParams);

		struct COMMANDER
		{
			wchar_t* wstrCommand;
			PARAMTYPE paramType;
			int countParams;
			COMMANDERFUNCTION funcCommander;
			int minAuthority;
			wchar_t* wstrHelpComment;
		};

	public:
		bool parseCommandLine(void *pOwner, NKWString& builder_command);

	public:
		virtual bool onExecuteCommand(void *pOwner, wchar_t* wstrCommand, wchar_t* wstrParams[MAX_PARAMETERS], wchar_t* longParam, int countParams) = 0;

	protected:
		const StringCommander::COMMANDER *_commands;
		const int _countCommands;
		const int _authority;

	public:
		StringCommander(const StringCommander::COMMANDER *commands, int countCommands, int authority);
		virtual ~StringCommander(void);
	};
}

#endif // __STRING_COMMANDER_HEADER__