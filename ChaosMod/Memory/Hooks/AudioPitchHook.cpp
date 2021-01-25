#include <stdafx.h>

static bool m_enabledHook = false;
static float m_targetPitch = 0;

__int64(*_OG_SetAudioPitch)(__int64* a1, int pitch);
__int64 _HK_SetAudioPitch(__int64* a1, int pitch)
{
	return _OG_SetAudioPitch(a1, m_enabledHook ? m_targetPitch : pitch);
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
	void SetAudioPitch(int pitch)
	{
		m_enabledHook = true;
		m_targetPitch = pitch;
	}

	void ResetAudioPitch()
	{
		m_enabledHook = false;
	}
}