#ifndef __CRASHREPORT_HEADER__
#define __CRASHREPORT_HEADER__
// @nolimitk
// 14.02.11
// crash report
// mini dump, stack walk, registers, CRT invalid parameter handler, pure function call handler

#pragma warning(disable: 4091)

#include "NKCore.h"
#include <dbghelp.h>

#define EXCEPTION_USER_DEFINED			0xC000FFFFL
#define DECLARE_LOG_VARIABLES()			DWORD __buffer_count__, __write_bytes__; char __buffer__[2048]
#define WRITECRASHLOG( h, fmt, ... )\
{\
	__buffer_count__ = sprintf_s( __buffer__, 2048, fmt, __VA_ARGS__);\
	if( __buffer_count__ > 0 ) { WriteFile( h, __buffer__, __buffer_count__, &__write_bytes__, NULL ); }\
} while( false )

#define LENGTH_USERDEFINED_EXCEPTION_INFO_LOG	4096

struct USERDEFINED_EXCEPTION_INFO
{
	const char *_strFile;
	int _line;
	wchar_t log[LENGTH_USERDEFINED_EXCEPTION_INFO_LOG];
};

#define CRASH(fmt, ...)		{ NE_ERRORLOG( L"[CRASH],!!!user defined crash!!!, "fmt, __VA_ARGS__ );\
	CrashReport::GetInstance()->RaiseCrash( __FILE__, __LINE__, fmt, __VA_ARGS__); }

// heap에서 처리하도록 한다.
class CrashReport : public NKCore::Singleton<CrashReport>
{
public:
	bool Create(void);
	LONG ExceptionFilter( PEXCEPTION_POINTERS pExceptionInfo );
	void ParameterHandler(const WCHAR *pExpression, const WCHAR *pFunction, const WCHAR *pFile, UINT Line, uintptr_t Reserved );
	void PureCallFunctionHanlder(void);
	void RaiseCrash(const char *file, int line, wchar_t *fmt, ...);
	
protected:
	void GenerateExceptionReport( HANDLE hReportFile, PEXCEPTION_POINTERS pExceptionInfo );
	void ImagehlpStackWalk( HANDLE hReportFile, PCONTEXT pContext );
public:
	void WriteMiniDump( PEXCEPTION_POINTERS pExceptionInfo );

protected:
	static LONG WINAPI UnhandledExceptionFilter( PEXCEPTION_POINTERS pExceptionInfo );
	inline static const WCHAR* GetExceptionString( DWORD dwCode );
	static BOOL GetLogicalAddress( PVOID addr, WCHAR * szModule, DWORD len, DWORD& section, ptrdiff_t& offset );
	static BOOL InitImagehlpFunctions(void);

	// for security-enhanced CRT functions, invalid parameter handler
	static void InvalidParameterHandler(const WCHAR *pExpression, const WCHAR *pFunction, const WCHAR *pFile, UINT Line, uintptr_t Reserved );

	// for pure fucntion call handler
	static void PureCallHandler(void);

protected:
	LPTOP_LEVEL_EXCEPTION_FILTER _previousFilter;
	wchar_t _strReportFileName[MAX_PATH];
	std::mutex _report_mutex;

	_invalid_parameter_handler _previousParameterHandler;
	_purecall_handler _previousPureCallHanlder;

protected:
	typedef BOOL (__stdcall * SYMINITIALIZEPROC)( HANDLE, LPSTR, BOOL );
	typedef BOOL (__stdcall *SYMCLEANUPPROC)( HANDLE );

	typedef BOOL (__stdcall * STACKWALKPROC)
					( DWORD, HANDLE, HANDLE, LPSTACKFRAME, LPVOID,
					PREAD_PROCESS_MEMORY_ROUTINE,PFUNCTION_TABLE_ACCESS_ROUTINE,
					PGET_MODULE_BASE_ROUTINE, PTRANSLATE_ADDRESS_ROUTINE );

#if defined _WIN64
	typedef LPVOID (__stdcall *SYMFUNCTIONTABLEACCESSPROC)( HANDLE, DWORD64 );
	typedef DWORD64 (__stdcall *SYMGETMODULEBASEPROC)( HANDLE, DWORD64 );
	typedef BOOL (__stdcall *SYMGETSYMFROMADDRPROC)( HANDLE, DWORD64, PDWORD64, PIMAGEHLP_SYMBOL64 );
	typedef BOOL (__stdcall *SYMGETLINEFROMADDRPROC)( HANDLE, DWORD64, PDWORD64, PIMAGEHLP_LINE64 );
#else
	typedef LPVOID (__stdcall *SYMFUNCTIONTABLEACCESSPROC)( HANDLE, DWORD );
	typedef DWORD (__stdcall *SYMGETMODULEBASEPROC)( HANDLE, DWORD );
	typedef BOOL (__stdcall *SYMGETSYMFROMADDRPROC)( HANDLE, DWORD, PDWORD, PIMAGEHLP_SYMBOL );
	typedef BOOL (__stdcall *SYMGETLINEFROMADDRPROC)( HANDLE, DWORD, PDWORD, PIMAGEHLP_LINE );
#endif

	typedef BOOL (__stdcall *SYMSETOPTIONSPROC)( DWORD );
	typedef BOOL (__stdcall *MINIDUMPWRITEDUMP)( HANDLE, DWORD, HANDLE, MINIDUMP_TYPE, PMINIDUMP_EXCEPTION_INFORMATION, PMINIDUMP_USER_STREAM_INFORMATION, PMINIDUMP_CALLBACK_INFORMATION );

	static SYMINITIALIZEPROC			_SymInitialize;
	static SYMCLEANUPPROC				_SymCleanup;
	static STACKWALKPROC				_StackWalk;
	static SYMFUNCTIONTABLEACCESSPROC	_SymFunctionTableAccess;
	static SYMGETMODULEBASEPROC			_SymGetModuleBase;
	static SYMGETSYMFROMADDRPROC		_SymGetSymFromAddr;
	static SYMGETLINEFROMADDRPROC		_SymGetLineFromAddr;
	static SYMSETOPTIONSPROC			_SymSetOptions;
	static MINIDUMPWRITEDUMP			_MiniDumpWriteDump;

public:
	CrashReport(void);
	virtual ~CrashReport(void);
};

#pragma warning(default: 4091)

#endif // __CRASHREPORT_HEADER__