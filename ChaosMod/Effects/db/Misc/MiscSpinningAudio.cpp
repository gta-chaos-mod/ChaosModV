#include "stdafx.h"

#include "Memory/Hooks/AudioPositionHook.h"
#include "Util/Types.h"

static float angle = 0.f;
static int speedMulti = 1.f;

static bool clockwise = true;

static void OnStart()
{
	speedMulti = g_Random.GetRandomInt(1, 5);
	clockwise = g_Random.GetRandomBool();
}

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	Hooks::SetAudioAngleFromEntity(player, angle);
	if (clockwise)
		angle += 1.f * speedMulti;
	else
		angle -= 1.f * speedMulti;

	if (angle > 359.f)
		angle = 0.f;
}

static void OnStop()
{
	angle = 0.f;
	Hooks::ResetAudioPostionHook();
}

static RegisterEffect registerEffect(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Spinning Audio",
		.Id = "misc_spinning_audio",
		.IsTimed = true,
		.IsShortDuration = true
	}
);