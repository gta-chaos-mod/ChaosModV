#include <stdafx.h>

#ifdef _DEBUG

__int64(*OG_sub_7FF788ADE188)();
__int64 HK_sub_7FF788ADE188()
{
	return 0;
}

static bool OnHook()
{
	Handle handle;

	//

	/*handle = Memory::FindPattern("E8 ? ? ? ? 4C 8B 4F 08 45 33 C0 66 45 39 41 ? 74 27 41 0F B7 49 ? 33 D2 8B C3 F7 F1 49 8B 01 48 8B 14 D0 EB 08 3B 1A 74 0B 48 8B 52 20 48 85 D2 75 F3 EB 04 4C 8D 42 08 49 8B C0 48 8B 5C 24 ? 48 83 C4 20 5F C3 33 C9");
	if (!handle.IsValid())
	{
		return false;
	}
	Memory::AddHook(handle.Into().Get<void>(), HK_sub_7FF788ADE188, &OG_sub_7FF788ADE188);*/

	//

	/*handle = Memory::FindPattern("48 8D 1D ? ? ? ? BF ? ? ? ? 48 81 EB ? ? ? ? 48 8B CB E8 ? ? ? ? FF CF 79 ED 48 8B 5C 24 ? 48 83 C4 20 5F C3 48 8D 05 ? ? ? ? 48 89 05 ? ? ? ? C3 CC 48 83 EC 28 66 83 3D");
	DWORD64 g_CControl = handle.At(2).Into().Addr();

	DWORD64 controls = g_CControl + 8398;
	/*BYTE* movUD = reinterpret_cast<BYTE*>(controls + 31 * 72);
	BYTE* movLR = reinterpret_cast<BYTE*>(controls + 30 * 72);

	BYTE bytes[72];
	for (int i = 0; i < 72; i++)
	{
		bytes[i] = movUD[i];
	}

	for (int i = 0; i < 72; i++)
	{
		movUD[i] = movLR[i];
	}

	for (int i = 0; i < 72; i++)
	{
		movLR[i] = bytes[i];
	}*/

	//

	return true;
}

static RegisterHook registerHook(OnHook, "TestHook");

#endif