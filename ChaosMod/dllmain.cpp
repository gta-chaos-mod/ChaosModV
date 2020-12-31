#include <stdafx.h>

#include "Main.h"

Main m_main;

void OnKeyboardInput(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow)
{
	m_main.OnKeyboardInput(key, repeats, scanCode, isExtended, isWithAlt, wasDownBefore, isUpNow);
}

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD reason, LPVOID lpReserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		Memory::Init();
		m_main.Init();

		scriptRegister(hInstance, []() { m_main.MainLoop(); });
		scriptRegisterAdditionalThread(hInstance, []() { m_main.RunEffectLoop(); });

		keyboardHandlerRegister(OnKeyboardInput);

		break;
	case DLL_PROCESS_DETACH:
		Memory::Uninit();

		scriptUnregister(hInstance);

		keyboardHandlerUnregister(OnKeyboardInput);

		break;
	}

	return TRUE;
}