#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <minidumpapiset.h>

#include <sstream>
typedef BOOL(WINAPI* _MiniDumpWriteDump)(HANDLE hProcess, DWORD ProcessId, HANDLE hFile, MINIDUMP_TYPE DumpType, PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, PMINIDUMP_CALLBACK_INFORMATION CallbackParam);


static LONG WINAPI CrashHandler(_EXCEPTION_POINTERS* exceptionInfo)
{
	HMODULE lib = LoadLibrary("dbghelp.dll");
	_MiniDumpWriteDump dumpFunc = reinterpret_cast<_MiniDumpWriteDump>(GetProcAddress(lib, "MiniDumpWriteDump"));
	SYSTEMTIME systemTime;
	GetSystemTime(&systemTime);

	CreateDirectory("chaosmod\\crashes", NULL);

	std::ostringstream fileName;
	fileName << "chaosmod\\crashes\\" << systemTime.wYear << "-" << systemTime.wMonth << "-" << systemTime.wDay
		<< "-" << systemTime.wHour << "-" << systemTime.wMinute << ".dmp";

	HANDLE file = CreateFile(fileName.str().c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	_MINIDUMP_EXCEPTION_INFORMATION exInfo;
	exInfo.ThreadId = GetCurrentThreadId();
	exInfo.ExceptionPointers = exceptionInfo;
	exInfo.ClientPointers = FALSE;

	DWORD flags = MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory;

	struct stat temp;
	if (stat("chaosmod\\.fulldumps", &temp) != -1)
	{
		flags = MiniDumpWithFullMemory | MiniDumpWithHandleData | MiniDumpWithModuleHeaders | MiniDumpWithUnloadedModules | MiniDumpWithProcessThreadData
			| MiniDumpWithFullMemoryInfo | MiniDumpWithThreadInfo;
	}

	dumpFunc(GetCurrentProcess(), GetCurrentProcessId(), file, static_cast<MINIDUMP_TYPE>(flags), &exInfo, NULL, NULL);

	CloseHandle(file);

	return EXCEPTION_CONTINUE_SEARCH;
}