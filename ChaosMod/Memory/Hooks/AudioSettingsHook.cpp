#include <stdafx.h>

#include "AudioSettingsHook.h"

#include "Memory/Hooks/Hook.h"

struct OverrideValue
{
	bool doOverride;
	int targetValue;
};

static OverrideValue ms_OverridePitch;
static OverrideValue ms_OverrideLPFCutoff;
static OverrideValue ms_OverrideHPFCutoff;
static OverrideValue ms_OverrideVolume;

__int16 sum(__int16 a, __int16 b)
{
	return (a > 5000 - b) ? 5000 : (a < -5000 - b) ? -5000 : (a + b);
}

void (*_OG_rage__audSound__CombineBuffers)(void *, __int64);
void _HK_rage__audSound__CombineBuffers(void *_this, __int64 combineBuffer)
{
	_OG_rage__audSound__CombineBuffers(_this, combineBuffer);

	if (ms_OverridePitch.doOverride)
	{
		__int16 *value = Handle(combineBuffer).At(IsEnhanced() ? 0x1C : 0xE).Get<__int16>();
		*value         = sum(*value, ms_OverridePitch.targetValue);
	}

	if (ms_OverrideLPFCutoff.doOverride)
	{
		__int16 *value = Handle(combineBuffer).At(IsEnhanced() ? 0x14 : 0x8).Get<__int16>();
		*value = std::min(*value, (short)ms_OverrideLPFCutoff.targetValue);
	}

	if (ms_OverrideHPFCutoff.doOverride)
	{
		__int16 *value = Handle(combineBuffer).At(IsEnhanced() ? 0x16 : 0xA).Get<__int16>();
		*value = std::max(*value, (short)ms_OverrideHPFCutoff.targetValue);
	}

	if (ms_OverrideVolume.doOverride)
	{
		int *value = Handle(combineBuffer).At(0).Get<int>();
		*value = ms_OverrideVolume.targetValue;
	}
}

static bool OnHook()
{
	auto handle = Memory::FindPattern("E8 ?? ?? ?? ?? 48 8B CE 44 89 7E 54", "E8 ?? ?? ?? ?? 44 89 77 54");
	if (!handle.IsValid())
		return false;

	Memory::AddHook(handle.Into().Get<void>(), _HK_rage__audSound__CombineBuffers, &_OG_rage__audSound__CombineBuffers);

	return true;
}

static RegisterHook registerHook(OnHook, nullptr, "rage::audSound::CombineBuffers");

namespace Hooks
{
	void SetAudioPitchFromSpeedMult(float speedMult)
	{
		SetAudioPitch(400.f * std::log2f(speedMult));
	}

	void SetAudioPitch(float pitch)
	{
		ms_OverridePitch.doOverride  = true;
		ms_OverridePitch.targetValue = (short)pitch;
	}

	float GetTargetAudioPitch()
	{
		return !ms_OverridePitch.doOverride ? 0.f : ms_OverridePitch.targetValue;
	}

	void ResetAudioPitch()
	{
		ms_OverridePitch.doOverride = false;
	}

	void SetAudioLPFCutoff(short value)
	{
		ms_OverrideLPFCutoff.doOverride  = true;
		ms_OverrideLPFCutoff.targetValue = value;
	}
	void ResetAudioLPFCutoff()
	{
		ms_OverrideLPFCutoff.doOverride = false;
	}

	void SetAudioHPFCutoff(short value)
	{
		ms_OverrideHPFCutoff.doOverride  = true;
		ms_OverrideHPFCutoff.targetValue = value;
	}
	void ResetAudioHPFCutoff()
	{
		ms_OverrideHPFCutoff.doOverride = false;
	}

	void SetAudioVolume(float value)
	{
		ms_OverrideVolume.doOverride  = true;
		ms_OverrideVolume.targetValue = *reinterpret_cast<int*>(&value);
	}
	void ResetAudioVolume()
	{
		ms_OverrideVolume.doOverride = false;
	}
}