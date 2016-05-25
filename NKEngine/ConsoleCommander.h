#ifndef __CONSOLE_COMMANDER_HEADER__
#define __CONSOLE_COMMANDER_HEADER__
// @nolimitk
// 14.12.08
// console commander
// executed at thread

#include "StringCommander.h"
#include "NKCore.h"

namespace NKEngine
{
	class ConsoleCommander : public NKCore::NKThread, public StringCommander
	{
	public:
		static bool start(void *pOwner, wchar_t *wstrParams[MAX_PARAMETERS], int countParams);
		static bool stop(void *pOwner, wchar_t *wstrParams[MAX_PARAMETERS], int countParams);
		static bool terminate(void *pOwner, wchar_t *wstrParams[MAX_PARAMETERS], int countParams);

	protected:
		enum COMMANDER_RESULTCODE
		{
			COMMANDER_RESULTCODE_SUCCESS,
			COMMANDER_RESULTCODE_ERROR,
			COMMANDER_RESULTCODE_WRONG_PARAMTYPE,
			COMMANDER_RESULTCODE_NOTMATCHED_COUNT_PARAMETERS,
			COMMANDER_RESULTCODE_UNKNOWN_COMMAND,
			COMMANDER_RESULTCODE_NOTHAVE_AUTHORITY
		};

	public:
		virtual bool onRun(void) override;
		virtual bool onExecuteCommand(void *pOwner, wchar_t *wstrCommand, wchar_t *wstrParams[MAX_PARAMETERS], wchar_t *longParam, int countParams) override;
		virtual void onCommandFailed(void *pOwner, int commander, wchar_t *wstrParams[MAX_PARAMETERS], int countParams, COMMANDER_RESULTCODE resultCode) = 0;

	protected:
		void* _pOwner;

	public:
		ConsoleCommander(void *pOwner, const StringCommander::COMMANDER *commands, int countCommands);
		virtual ~ConsoleCommander(void) {}
	};
}

#endif // __CONSOLE_COMMANDER_HEADER__