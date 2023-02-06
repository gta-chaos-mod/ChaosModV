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

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Maximap",
		.Id = "screen_maximap",
		.IsTimed = true,
		.IncompatibleWith = { "no_hud", "misc_flip_ui", "no_radar" }
	}
);