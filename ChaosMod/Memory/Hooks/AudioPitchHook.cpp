#include <stdafx.h>

static bool ms_bEnabledHook = false;
static float ms_fTargetPitch = 0;

__int64(*_OG_SetAudioPitch)(__int64* a1, int iPitch);
__int64 _HK_SetAudioPitch(__int64* a1, int pitch)
{
	return _OG_SetAudioPitch(a1, ms_bEnabledHook ? ms_fTargetPitch : pitch);
}

static bool OnHook()
{
	Handle handle = Memory::FindPattern("E8 ? ? ? ? 33 DB 49 8B FC");
	if (!handle.IsValid())
	{
		return false;
	}

	Memory::AddHook(handle.Into().Get<void>(), _HK_SetAudioPitch, &_OG_SetAudioPitch);

	return true;
}

static RegisterHook registerHook(OnHook, "_SetAudioPitch");

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