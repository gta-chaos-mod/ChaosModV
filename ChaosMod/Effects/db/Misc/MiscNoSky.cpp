#include <stdafx.h>

#include "Memory/Misc.h"

static void OnTick()
{
	Memory::SetSkyDisabled(true);
}

static void OnStop()
{
	Memory::SetSkyDisabled(false);
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "No Sky",
		.Id = "misc_nosky",
		.IsTimed = true
	}
);