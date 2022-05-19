#include "stdafx.h"

#include "Memory/Hooks/AudioPositionHook.h"
#include "Util/Types.h"

static float angle = 0.f;

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	Hooks::SetAudioAngleFromEntity(player, angle);
	angle += 4.f;
}

static void OnStop()
{
	angle = 0.f;
	Hooks::ResetAudioPostionHook();
}

static RegisterEffect registerEffect(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Spinning Audio",
		.Id = "misc_spinning_audio",
		.IsTimed = true,
		.IsShortDuration = true
	}
);