#include "stdafx.h"
#include "Hook.h"

static bool ms_bEnabledHook = false;
static float ms_fTargetVolume = 0;

__int64(*_OG_rage__audRequestedSettings__SetVolume)(__int64 _this, float fVolume);
__int64 _HK_rage__audRequestedSettings__SetVolume(__int64 _this, float volume)
{
	return _OG_rage__audRequestedSettings__SetVolume(_this, ms_bEnabledHook ? ms_fTargetVolume : volume);
}

static bool OnHook()
{
	Handle handle = Memory::FindPattern("E8 ? ? ? ? 4A 8B 84 EE ? ? ? ?");
	if (!handle.IsValid())
	{
		return false;
	}

	Memory::AddHook(handle.Into().Get<void>(), _HK_rage__audRequestedSettings__SetVolume, &_OG_rage__audRequestedSettings__SetVolume);

	return true;
}

static RegisterHook registerHook(OnHook, "rage__audRequestedSettings__SetVolume");

namespace Hooks
{
	void SetAudioVolume(float fVolume)
	{
		ms_fTargetVolume = fVolume;
		ms_bEnabledHook = true;
	}

	void ResetAudioVolume()
	{
		ms_bEnabledHook = false;
	}
}