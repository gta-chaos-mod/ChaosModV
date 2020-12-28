#include <stdafx.h>

static void SetControlStatus(int _control, bool _status)
{
	static void** a = nullptr;

	if (!a)
	{
		Handle handle = Memory::FindPattern("48 8D 05 ? ? ? ? 48 3B C7 0F 84 ? ? ? ? 48 8D 8F ? ? ? ? 48 8D 15 ? ? ? ? E8 ? ? ? ? 48 8D 8F ? ? ? ? 48 8D 15 ? ? ? ? 0F 28 F0 0F 57 35 ? ? ? ? E8");
		
		a = handle.At(2).Into().Value<void**>();
	}

	*reinterpret_cast<BYTE*>(a + 72 * _control + 33762) = _status ? 255 : 0;
}

static void OnStart()
{
	SetControlStatus(22, false);

	return;

	for (int i = 0; i < 357; i++)
	{
		SetControlStatus(i, false);
	}
}

static void OnStop()
{
	SetControlStatus(22, true);

	return;

	for (int i = 0; i < 357; i++)
	{
		SetControlStatus(i, true);
	}
}

//static RegisterEffect registerEffect(EFFECT_INVERT_CONTROLS, OnStart, OnStop);