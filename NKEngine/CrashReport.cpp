#include "CrashReport.h"
#include "NKEngineLog.h"

CrashReport::SYMINITIALIZEPROC CrashReport::_SymInitialize = 0;
CrashReport::SYMCLEANUPPROC CrashReport::_SymCleanup = 0;
CrashReport::STACKWALKPROC CrashReport::_StackWalk = 0;
CrashReport::SYMFUNCTIONTABLEACCESSPROC CrashReport::_SymFunctionTableAccess = 0;
CrashReport::SYMGETMODULEBASEPROC CrashReport::_SymGetModuleBase = 0;
CrashReport::SYMGETSYMFROMADDRPROC CrashReport::_SymGetSymFromAddr = 0;
CrashReport::SYMGETLINEFROMADDRPROC CrashReport::_SymGetLineFromAddr = 0;
CrashReport::SYMSETOPTIONSPROC CrashReport::_SymSetOptions = 0;
CrashReport::MINIDUMPWRITEDUMP CrashReport::_MiniDumpWriteDump = 0;

CrashReport::CrashReport(void)
	:_previousFilter(nullptr)
	,_previousParameterHandler(nullptr)
	,_previousPureCallHanlder(nullptr)
{
}

CrashReport::~CrashReport(void)
{
}

bool CrashReport::Create(void)
{
	_previousFilter = SetUnhandledExceptionFilter( UnhandledExceptionFilter );

	GetModuleFileName( 0, _strReportFileName, MAX_PATH );

	// chage filename ".exe" to ".rpt"
	wchar_t *pDot = wcsrchr( _strReportFileName, L'.' );
	if( pDot != NULL )
	{
		pDot++;
		*pDot = NULL;
		wcsncat_s( _strReportFileName, L"rpt", 3 );
	}

	_previousParameterHandler = _set_invalid_parameter_handler(InvalidParameterHandler);
	_previousPureCallHanlder = _set_purecall_handler( PureCallHandler );

	return true;
}

LONG CrashReport::ExceptionFilter( PEXCEPTION_POINTERS pExceptionInfo )
{
	NKENGINELOG_ERROR( L"[CRASH],!!!crash report!!!" );

	std::lock_guard<std::mutex> _lock(_report_mutex);

	if( pExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW )
	{
		NKENGINELOG_ERROR( L"[CRASH],!!!stack overflow!!!" );
	}

	NKENGINELOG_ERROR( L"[CRASH],exception, %08X, %s", pExceptionInfo->ExceptionRecord->ExceptionCode, GetExceptionString(pExceptionInfo->ExceptionRecord->ExceptionCode) );
	NKENGINELOG_ERROR( L"[CRASH],thread, %d", GetCurrentThreadId() );

	// heap을 사용한다.
	HANDLE hReportFile = CreateFile( _strReportFileName,
								GENERIC_WRITE,
								0,
								0,
								OPEN_ALWAYS,
								FILE_FLAG_WRITE_THROUGH,
								0 );

	if( hReportFile != INVALID_HANDLE_VALUE )
	{
		SetFilePointer( hReportFile, 0, 0, FILE_END );

		GenerateExceptionReport( hReportFile, pExceptionInfo );

		CloseHandle( hReportFile );
	}

	// log thread에서 crash가 발생할 수 있다.
	NKLog::AsyncLogSingleton::destroy();

	if( _previousFilter != NULL )
		return _previousFilter( pExceptionInfo );
	else
		return EXCEPTION_CONTINUE_SEARCH;
}

void CrashReport::ParameterHandler(const WCHAR *pExpression, const WCHAR *pFunction, const WCHAR *pFile, UINT Line, uintptr_t Reserved )
{
	NKENGINELOG_ERROR( L"[CRASH],!!!invalid parameter handler!!!" );

	std::lock_guard<std::mutex> _lock(_report_mutex);

	HANDLE hReportFile = CreateFile( _strReportFileName,
								GENERIC_WRITE,
								0,
								0,
								OPEN_ALWAYS,
								FILE_FLAG_WRITE_THROUGH,
								0 );

	if( hReportFile != INVALID_HANDLE_VALUE )
	{
		SetFilePointer( hReportFile, 0, 0, FILE_END );

		DECLARE_LOG_VARIABLES();

		WRITECRASHLOG( hReportFile, "//=====================================================\r\n" );

		SYSTEMTIME systemTime;
		GetLocalTime( &systemTime );

		WRITECRASHLOG( hReportFile, "Invalid Parameters occurred on %d/%02d/%02d %02d:%02d\r\n", systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute );

		WRITECRASHLOG( hReportFile, "ERROR NO:\t%d\r\n", errno );

		if( pExpression != NULL )
		{
			WRITECRASHLOG( hReportFile, "EXPRESSION:\t%S\r\n", pExpression );
		}

		if( pFunction != NULL )
		{
			WRITECRASHLOG( hReportFile, "FUNCTION:\t%S\r\n", pFunction );
		}

		if( pFile != NULL )
		{
			WRITECRASHLOG( hReportFile, "FILENAME:\t%S\r\n", pFile );
		}

		WRITECRASHLOG( hReportFile, "LINE:\t%d\r\n", Line );

		CloseHandle( hReportFile );
	}

	if( _previousParameterHandler )
	{
		_previousParameterHandler( pExpression, pFunction, pFile, Line, Reserved );
	}
}

void CrashReport::PureCallFunctionHanlder(void)
{
	NKENGINELOG_ERROR( L"[CRASH],!!!pure call function handler!!!" );

	std::lock_guard<std::mutex> _lock(_report_mutex);

	HANDLE hReportFile = CreateFile( _strReportFileName,
								GENERIC_WRITE,
								0,
								0,
								OPEN_ALWAYS,
								FILE_FLAG_WRITE_THROUGH,
								0 );

	if( hReportFile != INVALID_HANDLE_VALUE )
	{
		SetFilePointer( hReportFile, 0, 0, FILE_END );

		DECLARE_LOG_VARIABLES();

		WRITECRASHLOG( hReportFile, "//=====================================================\r\n" );

		SYSTEMTIME systemTime;
		GetLocalTime( &systemTime );

		WRITECRASHLOG( hReportFile, "Pure call function occurred on %d/%02d/%02d %02d:%02d\r\n", systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute );

		if( NKCore::g_indexCallstack > 0 )
		{
			WRITECRASHLOG( hReportFile, "guard callstack\r\n" );
			for( int i = NKCore::g_indexCallstack-1 ; i >= 0 ; i-- )
			{
				WRITECRASHLOG( hReportFile, "%S\r\n", NKCore::g_customCallstack[i] );
			}
		}
		else
		{
			WRITECRASHLOG( hReportFile, "there is no guard callstack, stack index %d\r\n", NKCore::g_indexCallstack );
		}

		CloseHandle( hReportFile );
	}

	if( _previousPureCallHanlder )
	{
		_previousPureCallHanlder();
	}
}

void CrashReport::RaiseCrash(const char *file, int line, wchar_t *fmt, ...)
{
	va_list args;
	USERDEFINED_EXCEPTION_INFO *info = (USERDEFINED_EXCEPTION_INFO *)HeapAlloc(GetProcessHeap(), 0, sizeof(USERDEFINED_EXCEPTION_INFO));

	info->_strFile	= file;
	info->_line	= line;
	va_start(args, fmt);
	vswprintf(info->log, _countof(info->log), fmt, args);
	va_end(args);

	RaiseException( EXCEPTION_USER_DEFINED, EXCEPTION_NONCONTINUABLE, 1, (const ULONG_PTR *)&info);
}

void CrashReport::GenerateExceptionReport( HANDLE hReportFile, PEXCEPTION_POINTERS pExceptionInfo )
{
	DECLARE_LOG_VARIABLES();

	// Start out with a banner
	WRITECRASHLOG( hReportFile, "//=====================================================\r\n" );

    PEXCEPTION_RECORD pExceptionRecord = pExceptionInfo->ExceptionRecord;

	SYSTEMTIME systemTime;
	wchar_t strCurrentTime[64];
	GetLocalTime( &systemTime );

	wsprintf( strCurrentTime, L"%d/%02d/%02d %02d:%02d", systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute );

	WRITECRASHLOG( hReportFile, "Crash occurred on %S\r\n", strCurrentTime );

    // First print information about the type of fault
    WRITECRASHLOG( hReportFile, "Exception code:\t%08X\t%S\r\n",
                pExceptionRecord->ExceptionCode,
                GetExceptionString(pExceptionRecord->ExceptionCode) );

    // Now print information about where the fault occured
    WCHAR szFaultingModule[MAX_PATH];
    DWORD section;
	ptrdiff_t offset;
    if( GetLogicalAddress(  pExceptionRecord->ExceptionAddress, szFaultingModule, sizeof( szFaultingModule ), section, offset ) == TRUE )
	{
		WRITECRASHLOG( hReportFile, "Fault address:\t%08p\t%04X:%016tX\t%S\r\n", pExceptionRecord->ExceptionAddress, section, offset, szFaultingModule );
	}

    PCONTEXT pCtx = pExceptionInfo->ContextRecord;

    // dump the registers
#ifdef _M_IX86  // Intel Only!
    WRITECRASHLOG( hReportFile, "\r\nRegisters:\r\n" );

    WRITECRASHLOG( hReportFile, "EAX:%08X\r\nEBX:%08X\r\nECX:%08X\r\nEDX:%08X\r\nESI:%08X\r\nEDI:%08X\r\n",
            pCtx->Eax, pCtx->Ebx, pCtx->Ecx, pCtx->Edx, pCtx->Esi, pCtx->Edi );

    WRITECRASHLOG( hReportFile, "CS:EIP:%04X:%08X\r\n", pCtx->SegCs, pCtx->Eip );
    WRITECRASHLOG( hReportFile, "SS:ESP:%04X:%08X  EBP:%08X\r\n",
                pCtx->SegSs, pCtx->Esp, pCtx->Ebp );
    WRITECRASHLOG( hReportFile, "DS:%04X  ES:%04X  FS:%04X  GS:%04X\r\n",
                pCtx->SegDs, pCtx->SegEs, pCtx->SegFs, pCtx->SegGs );
	WRITECRASHLOG( hReportFile, "Flags:\t%08X\r\n", pCtx->EFlags );
#elif defined _M_AMD64
	WRITECRASHLOG( hReportFile,
		"RAX=%016llX  RBX=%016llX  RCX=%016llX  RDX=%016llX\r\n"
		"RSI=%016llX  RDI=%016llX  RBP=%016llX  RSP=%016llX\r\n"
		"RIP=%016llX  FLG=%016X   R8=%016llX   R9=%016llX\r\n"
		"R10=%016llX  R11=%016llX  R12=%016llX  R13=%016llX\r\n"
		"R14=%016llX  R15=%016llX\r\n",
		pCtx->Rax, 
		pCtx->Rbx,
		pCtx->Rcx,
		pCtx->Rdx,
		pCtx->Rsi,
		pCtx->Rdi,
		pCtx->Rbp,
		pCtx->Rsp,
		pCtx->Rip,
		pCtx->EFlags,
		pCtx->R8,
		pCtx->R9,
		pCtx->R10,
		pCtx->R11,
		pCtx->R12,
		pCtx->R13,
		pCtx->R14,
		pCtx->R15);
#endif

	if( InitImagehlpFunctions() != TRUE )
	{
		OutputDebugString( L"DbgHelp.DLL or its exported procs not found" );
		return;
	}

	// mini dump
	WriteMiniDump( pExceptionInfo );
	
	if( pExceptionRecord->ExceptionCode == EXCEPTION_USER_DEFINED && pExceptionRecord->NumberParameters > 0 )
	{
		USERDEFINED_EXCEPTION_INFO *DefinedExceptionInfo = (USERDEFINED_EXCEPTION_INFO *)pExceptionRecord->ExceptionInformation[0];
		WRITECRASHLOG( hReportFile, "\r\nUser defined Exception : %s, %d\r\n", DefinedExceptionInfo->_strFile, DefinedExceptionInfo->_line );
		WRITECRASHLOG( hReportFile, "Exception Description : %S\r\n", DefinedExceptionInfo->log );
	}

	ImagehlpStackWalk( hReportFile, pCtx );

    _SymCleanup( GetCurrentProcess() );

    WRITECRASHLOG( hReportFile, "\r\n" );
}

void CrashReport::WriteMiniDump( PEXCEPTION_POINTERS pExceptionInfo )
{
	SYSTEMTIME systemTime;
	wchar_t strCurrentTime[64];
	GetLocalTime( &systemTime );

	wsprintf( strCurrentTime, L"_%d%02d%02d_%02d%02d%02d.dmp", systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond );

	wchar_t strDumpFileName[MAX_PATH];
	GetModuleFileName( 0, strDumpFileName, MAX_PATH );
	wchar_t *pDot = wcsrchr( strDumpFileName, L'.' );
	if( pDot != NULL )
	{
		*pDot = NULL;
		wcsncat_s( strDumpFileName, strCurrentTime, wcslen(strCurrentTime) );
	}

	HANDLE hDumpFile = CreateFile( strDumpFileName,
										GENERIC_READ | GENERIC_WRITE,
										0,
										0,
										CREATE_ALWAYS,
										FILE_ATTRIBUTE_NORMAL,
										0 );

	if( hDumpFile != INVALID_HANDLE_VALUE )
	{
		MINIDUMP_EXCEPTION_INFORMATION mdei;

		mdei.ThreadId = GetCurrentThreadId();
		mdei.ExceptionPointers = pExceptionInfo;
		mdei.ClientPointers = FALSE;

		_MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hDumpFile,
			(MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithDataSegs | MiniDumpWithFullMemory | MiniDumpWithHandleData), &mdei, NULL, NULL);
				
		CloseHandle(hDumpFile);
	}
}

void CrashReport::ImagehlpStackWalk( HANDLE hReportFile, PCONTEXT pContext )
{
	DECLARE_LOG_VARIABLES();

    WRITECRASHLOG( hReportFile, "\r\nCall stack:\r\n" );

    WRITECRASHLOG( hReportFile, "Address\tFrame\tLocal Address\tModule/Source\r\n" );

    // Could use SymSetOptions here to add the SYMOPT_DEFERRED_LOADS flag

    STACKFRAME sf;
    memset( &sf, 0, sizeof(sf) );

    // Initialize the STACKFRAME structure for the first call.  This is only
    // necessary for Intel CPUs, and isn't mentioned in the documentation.
#if defined _WIN64
	#define MACHINE_TYPE	IMAGE_FILE_MACHINE_AMD64
	sf.AddrPC.Offset       = pContext->Rip;
    sf.AddrPC.Mode         = AddrModeFlat;
    sf.AddrStack.Offset    = pContext->Rsp;
    sf.AddrStack.Mode      = AddrModeFlat;
    sf.AddrFrame.Offset    = pContext->Rbp;
    sf.AddrFrame.Mode      = AddrModeFlat;
#else
	#define MACHINE_TYPE	IMAGE_FILE_MACHINE_I386
	sf.AddrPC.Offset       = pContext->Eip;
    sf.AddrPC.Mode         = AddrModeFlat;
    sf.AddrStack.Offset    = pContext->Esp;
    sf.AddrStack.Mode      = AddrModeFlat;
    sf.AddrFrame.Offset    = pContext->Ebp;
    sf.AddrFrame.Mode      = AddrModeFlat;
#endif

    while ( 1 )
    {
        if ( _StackWalk( MACHINE_TYPE,
                            GetCurrentProcess(),
                            GetCurrentThread(),
                            &sf,
                            pContext,
                            0,
                            _SymFunctionTableAccess,
                            _SymGetModuleBase,
                            0 ) != TRUE )
            break;

        if ( 0 == sf.AddrFrame.Offset ) // Basic sanity check to make sure
            break;                      // the frame is OK.  Bail if not.

        WRITECRASHLOG( hReportFile, "%016tX\t%016tX\t", sf.AddrPC.Offset, sf.AddrFrame.Offset );

        // IMAGEHLP is wacky, and requires you to pass in a pointer to a
        // IMAGEHLP_SYMBOL structure.  The problem is that this structure is
        // variable length.  That is, you determine how big the structure is
        // at runtime.  This means that you can't use sizeof(struct).
        // So...make a buffer that's big enough, and make a pointer
        // to the buffer.  We also need to initialize not one, but TWO
        // members of the structure before it can be used.

        BYTE symbolBuffer[ sizeof(IMAGEHLP_SYMBOL) + 512 ];
        PIMAGEHLP_SYMBOL pSymbol = (PIMAGEHLP_SYMBOL)symbolBuffer;
        pSymbol->SizeOfStruct = sizeof(symbolBuffer);
        pSymbol->MaxNameLength = 512;

#if defined _WIN64
		DWORD64 symDisplacement = 0;  // Displacement of the input address,
                                    // relative to the start of the symbol
		DWORD64 LineDisplacement = 0;
#else
        DWORD symDisplacement = 0;  // Displacement of the input address,
                                    // relative to the start of the symbol
		DWORD LineDisplacement = 0;
#endif
		
		// by nolimitk. 06.05.22
		IMAGEHLP_LINE ImageLine;
		ZeroMemory( &ImageLine, sizeof(IMAGEHLP_LINE) );
		ImageLine.SizeOfStruct = sizeof(IMAGEHLP_LINE);

        if ( _SymGetSymFromAddr(GetCurrentProcess(), sf.AddrPC.Offset, &symDisplacement, pSymbol) == TRUE )
        {
			if( _SymGetLineFromAddr(GetCurrentProcess(), sf.AddrPC.Offset, &LineDisplacement, &ImageLine ) == TRUE )
			{
				// @nolimitk. 11.05.06
				// filename이 wchar_t가 아니다.
				WRITECRASHLOG( hReportFile, "%hs+0x%016llX\t%s+%d\t%X\r\n", pSymbol->Name, symDisplacement, ImageLine.FileName, ImageLine.LineNumber, GetLastError() );
			}
			else
			{
				WRITECRASHLOG( hReportFile, "%hs+0x%016llX\t%X\r\n", pSymbol->Name, symDisplacement, GetLastError() );
			}            
        }
        else
        {
			// No symbol found.  Print out the logical address instead.
            WCHAR szModule[MAX_PATH] = L"";
            DWORD section = 0;
			ptrdiff_t offset = 0;

            GetLogicalAddress( (PVOID)sf.AddrPC.Offset, szModule, sizeof(szModule), section, offset );

            WRITECRASHLOG( hReportFile, "%04X:%016tX\t%S\t%X\r\n", section, offset, szModule, GetLastError() );
        }
    }
}

LONG WINAPI CrashReport::UnhandledExceptionFilter( PEXCEPTION_POINTERS pExceptionInfo )
{
	return getInstance()->ExceptionFilter( pExceptionInfo );
}

void CrashReport::InvalidParameterHandler(const WCHAR *pExpression, const WCHAR *pFunction, const WCHAR *pFile, UINT Line, uintptr_t Reserved )
{
	return getInstance()->ParameterHandler( pExpression, pFunction, pFile, Line, Reserved );
}

void CrashReport::PureCallHandler(void)
{
	return getInstance()->PureCallFunctionHanlder();
}

const WCHAR* CrashReport::GetExceptionString( DWORD dwCode )
{
	#define EXCEPTION( x ) case EXCEPTION_##x: return L#x;

    switch ( dwCode )
    {
        EXCEPTION( ACCESS_VIOLATION )
        EXCEPTION( DATATYPE_MISALIGNMENT )
        EXCEPTION( BREAKPOINT )
        EXCEPTION( SINGLE_STEP )
        EXCEPTION( ARRAY_BOUNDS_EXCEEDED )
        EXCEPTION( FLT_DENORMAL_OPERAND )
        EXCEPTION( FLT_DIVIDE_BY_ZERO )
        EXCEPTION( FLT_INEXACT_RESULT )
        EXCEPTION( FLT_INVALID_OPERATION )
        EXCEPTION( FLT_OVERFLOW )
        EXCEPTION( FLT_STACK_CHECK )
        EXCEPTION( FLT_UNDERFLOW )
        EXCEPTION( INT_DIVIDE_BY_ZERO )
        EXCEPTION( INT_OVERFLOW )
        EXCEPTION( PRIV_INSTRUCTION )
        EXCEPTION( IN_PAGE_ERROR )
        EXCEPTION( ILLEGAL_INSTRUCTION )
        EXCEPTION( NONCONTINUABLE_EXCEPTION )
        EXCEPTION( STACK_OVERFLOW )
        EXCEPTION( INVALID_DISPOSITION )
        EXCEPTION( GUARD_PAGE )
        EXCEPTION( INVALID_HANDLE )
		EXCEPTION( USER_DEFINED )
    }

    // If not one of the "known" exceptions, try to get the string
    // from NTDLL.DLL's message table.
    static WCHAR szBuffer[512] = { 0 };

    FormatMessage(  FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE,
                    GetModuleHandle( L"NTDLL.DLL" ),
                    dwCode, 0, szBuffer, sizeof( szBuffer ), 0 );

    return szBuffer;
}

BOOL CrashReport::GetLogicalAddress( PVOID addr, WCHAR *szModule, DWORD len, DWORD& section, ptrdiff_t& offset )
{
    MEMORY_BASIC_INFORMATION mbi;

    if ( !VirtualQuery( addr, &mbi, sizeof(MEMORY_BASIC_INFORMATION) ) )
        return FALSE;

    if ( !GetModuleFileName( (HMODULE)mbi.AllocationBase, szModule, len ) )
        return FALSE;

	//HMODULE hMod = (HMODULE)(mbi.AllocationBase);

    // Point to the DOS header in memory
    PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)mbi.AllocationBase;

    // From the DOS header, find the NT (PE) header
    PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)((ptrdiff_t)mbi.AllocationBase + (ptrdiff_t)pDosHdr->e_lfanew);

    PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION( pNtHdr );

	ptrdiff_t rva = ((ptrdiff_t)addr - (ptrdiff_t)mbi.AllocationBase); // RVA is offset from module load address

    // Iterate through the section table, looking for the one that encompasses
    // the linear address.
    for( unsigned i = 0; i < pNtHdr->FileHeader.NumberOfSections ; i++, pSection++ )
    {
        ptrdiff_t sectionStart = pSection->VirtualAddress;
        ptrdiff_t sectionEnd = sectionStart + max(pSection->SizeOfRawData, pSection->Misc.VirtualSize);

        // Is the address in this section???
        if ( (rva >= sectionStart) && (rva <= sectionEnd) )
        {
            // Yes, address is in the section.  Calculate section and offset,
            // and store in the "section" & "offset" params, which were
            // passed by reference.
            section = i+1;
            offset = rva - sectionStart;
            return TRUE;
        }
    }

    return FALSE;   // Should never get here!
}

BOOL CrashReport::InitImagehlpFunctions(void)
{
	HMODULE hModImagehlp = LoadLibrary( L"DbgHelp.DLL" );

    if ( !hModImagehlp )
        return FALSE;

    _SymInitialize = (SYMINITIALIZEPROC)GetProcAddress( hModImagehlp, "SymInitialize" );
    if ( !_SymInitialize )
        return FALSE;

    _SymCleanup = (SYMCLEANUPPROC)GetProcAddress( hModImagehlp, "SymCleanup" );
    if ( !_SymCleanup )
        return FALSE;

#if defined _WIN64
	_StackWalk = (STACKWALKPROC)GetProcAddress( hModImagehlp, "StackWalk64" );
    if ( !_StackWalk )
        return FALSE;

	_SymFunctionTableAccess = (SYMFUNCTIONTABLEACCESSPROC)GetProcAddress( hModImagehlp, "SymFunctionTableAccess64" );
    if ( !_SymFunctionTableAccess )
        return FALSE;

	_SymGetModuleBase = (SYMGETMODULEBASEPROC)GetProcAddress( hModImagehlp, "SymGetModuleBase64" );
    if ( !_SymGetModuleBase )
        return FALSE;

	_SymGetSymFromAddr = (SYMGETSYMFROMADDRPROC)GetProcAddress( hModImagehlp, "SymGetSymFromAddr64" );
    if ( !_SymGetSymFromAddr )
        return FALSE;

	// by nolimitk.
	_SymGetLineFromAddr = (SYMGETLINEFROMADDRPROC)GetProcAddress( hModImagehlp, "SymGetLineFromAddr64" );
	if( !_SymGetLineFromAddr )
	{
		return FALSE;
	}
#else
    _StackWalk = (STACKWALKPROC)GetProcAddress( hModImagehlp, "StackWalk" );
    if ( !_StackWalk )
        return FALSE;

	_SymFunctionTableAccess = (SYMFUNCTIONTABLEACCESSPROC)GetProcAddress( hModImagehlp, "SymFunctionTableAccess" );
    if ( !_SymFunctionTableAccess )
        return FALSE;

	_SymGetModuleBase = (SYMGETMODULEBASEPROC)GetProcAddress( hModImagehlp, "SymGetModuleBase" );
    if ( !_SymGetModuleBase )
        return FALSE;

	_SymGetSymFromAddr = (SYMGETSYMFROMADDRPROC)GetProcAddress( hModImagehlp, "SymGetSymFromAddr" );
    if ( !_SymGetSymFromAddr )
        return FALSE;

	// by nolimitk.
	_SymGetLineFromAddr = (SYMGETLINEFROMADDRPROC)GetProcAddress( hModImagehlp, "SymGetLineFromAddr" );
	if( !_SymGetLineFromAddr )
	{
		return FALSE;
	}
#endif
	
	_SymSetOptions = (SYMSETOPTIONSPROC)GetProcAddress( hModImagehlp, "SymSetOptions" );
	if( !_SymSetOptions )
	{
		return FALSE;
	}

	_MiniDumpWriteDump = (MINIDUMPWRITEDUMP)GetProcAddress( hModImagehlp, "MiniDumpWriteDump" );
	if( !_MiniDumpWriteDump )
	{
		return FALSE;
	}

	// GetLine을 하기위해서 반드시 설정을 해주어야 한다.
	_SymSetOptions( (SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME) );
	//

    if( _SymInitialize( GetCurrentProcess(), 0, TRUE ) == FALSE )
        return FALSE;

    return TRUE;        
}