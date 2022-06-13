#include <stdafx.h>

#include "Memory/Hooks/Hook.h"

static bool ms_bEnabledHook  = false;
static float ms_fTargetPitch = 0;

__int64 (*_OG_rage__audRequestedSettings__SetPitch)(__int64 _this, int iPitch);
__int64 _HK_rage__audRequestedSettings__SetPitch(__int64 _this, int pitch)
{
	return _OG_rage__audRequestedSettings__SetPitch(_this, ms_bEnabledHook ? ms_fTargetPitch : pitch);
}

static bool OnHook()
{
	Handle handle =
	    Memory::FindPattern("E8 ? ? ? ? 49 8B 8F 00 31 00 00 0F B6 81 80 00 00 00 3D FF 00 00 00 74 25 0F AF 05");
	if (!handle.IsValid())
	{
		return false;
	}

	Memory::AddHook(handle.Into().Get<void>(), _HK_rage__audRequestedSettings__SetPitch,
	                &_OG_rage__audRequestedSettings__SetPitch);

	return true;
}

static RegisterHook registerHook(OnHook, nullptr, "rage::audRequestedSettings::SetPitch");

namespace Hooks
{
	void SetAudioPitch(int iPitch)
	{
		ms_bEnabledHook = true;
		ms_fTargetPitch = iPitch;
	}

	void ResetAudioPitch()
	{
		ms_bEnabledHook = false;
	}
}