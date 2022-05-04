/*
	Effect by Reguas
*/

#include <stdafx.h>
#include "Memory/UI.h"

static void OnTick()
{
	Memory::MultiplyRadarSize(5.4f, 0.1f);
}

static void OnStop()
{
	Memory::ResetRadar();
}

static RegisterEffect registerEffect(EFFECT_MISC_MAXIMAP, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Maximap",
		.Id = "misc_maximap",
		.IsTimed = true,
		.IncompatibleWith = { EFFECT_NO_HUD, EFFECT_MISC_FLIP_UI }
	}
);