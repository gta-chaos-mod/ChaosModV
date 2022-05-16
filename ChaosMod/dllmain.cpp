#include "Main.h"
#include <stdafx.h>

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		SetUnhandledExceptionFilter(CrashHandler);

		Memory::Init();

		scriptRegister(hInstance, Main::OnRun);

		keyboardHandlerRegister(Main::OnKeyboardInput);

		presentCallbackRegister((PresentCallback)Main::OnPresent);

		break;
	case DLL_PROCESS_DETACH:
		Memory::Uninit();
		Main::OnCleanup();

		scriptUnregister(hInstance);

		keyboardHandlerUnregister(Main::OnKeyboardInput);

		presentCallbackUnregister((PresentCallback)Main::OnPresent);

		if (GetConsoleWindow())
		{
			g_ConsoleOut.close();

			FreeConsole();
		}

		break;
	}

	return TRUE;
}