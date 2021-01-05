#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <minidumpapiset.h>

#include <sstream>

static LONG WINAPI CrashHandler(_EXCEPTION_POINTERS* exceptionInfo)
{
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

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file, static_cast<MINIDUMP_TYPE>(flags), &exInfo, NULL, NULL);

	CloseHandle(file);

	return EXCEPTION_CONTINUE_SEARCH;
}