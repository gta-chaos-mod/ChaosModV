#include <stdafx.h>

#include "AudioClearnessHook.h"

#include "Memory/Hooks/Hook.h"

static bool ms_EnabledHook = false;
static int ms_Value        = 0;

__int64 (*_OG_rage__audRequestedSettings__SetClearness)(__int64 _this, __int64 clearness);
__int64 _HK_rage__audRequestedSettings__SetClearness(__int64 _this, __int64 clearness)
{
	return _OG_rage__audRequestedSettings__SetClearness(_this, ms_EnabledHook ? ms_Value : clearness);
}

static bool OnHook()
{
	Handle handle = Memory::FindPattern("E8 ? ? ? ? 83 64 24 ? ? 49 8B 8C 36 ? ? ? ? 45 33 C9");
	if (!handle.IsValid())
	{
		return false;
	}

	Memory::AddHook(handle.Into().Get<void>(), _HK_rage__audRequestedSettings__SetClearness,
	                &_OG_rage__audRequestedSettings__SetClearness);

	return true;
}

static RegisterHook registerHook(OnHook, nullptr, "rage::audRequestedSettings::SetClearness");

namespace Hooks
{
	void SetAudioClearness(int value)
	{
		ms_EnabledHook = true;
		ms_Value       = value;
	}

	void ResetAudioClearness()
	{
		ms_EnabledHook = false;
	}
}
