#include "stdafx.h"
#include "Main.h"

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		scriptRegister(hInstance, Main::Init);
		keyboardHandlerRegister(Main::OnKeyboardInput);

		break;

	case DLL_PROCESS_DETACH:
		Main::Stop();
		scriptUnregister(hInstance);
		keyboardHandlerUnregister(Main::OnKeyboardInput);

		break;
	}
	return TRUE;
}