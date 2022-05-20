#include <stdafx.h>

#include "Main.h"

#include "Memory/Memory.h"

#include "Util/CrashHandler.h"

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		SetUnhandledExceptionFilter(CrashHandler);

		Memory::Init();

		scriptRegister(hInstance, Main::OnRun);

		keyboardHandlerRegister(Main::OnKeyboardInput);

		break;
	case DLL_PROCESS_DETACH:
		Main::OnCleanup();
		Memory::Uninit();

		scriptUnregister(hInstance);

		keyboardHandlerUnregister(Main::OnKeyboardInput);

		if (GetConsoleWindow())
		{
			g_ConsoleOut.close();

			FreeConsole();
		}

		break;
	}

	return TRUE;
}