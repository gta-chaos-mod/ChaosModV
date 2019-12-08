#include "stdafx.h"
#include "Main.h"
#include "Memory.h"
#include <memory>

Main m_main;
bool m_isInitialized = false;

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

		if (!m_main.Init())
		{
			return FALSE;
		}

		scriptRegister(hInstance, []() { m_main.MainLoop(); });
		scriptRegisterAdditionalThread(hInstance, []() { m_main.RunEffectLoop(); });

		keyboardHandlerRegister(OnKeyboardInput);

		m_isInitialized = true;

		break;
	case DLL_PROCESS_DETACH:
		Memory::Stop();

		if (m_isInitialized)
		{
			scriptUnregister(hInstance);

			keyboardHandlerUnregister(OnKeyboardInput);
		}

		break;
	}

	return TRUE;
}