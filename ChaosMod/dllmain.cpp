#include "stdafx.h"
#include "Main.h"
#include "Memory.h"

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved)
{
	static HANDLE threadHandle;

	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		Memory::Init();

		if (!Main::Init())
		{
			return FALSE;
		}

		scriptRegister(hInstance, Main::Loop);

		keyboardHandlerRegister(Main::OnKeyboardInput);

		break;
	case DLL_PROCESS_DETACH:
		Main::Stop();
		Memory::Stop();

		scriptUnregister(hInstance);

		keyboardHandlerUnregister(Main::OnKeyboardInput);

		break;
	}

	return TRUE;
}