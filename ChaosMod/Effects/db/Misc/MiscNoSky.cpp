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
static RegisterEffect registerEffect(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "No Sky",
		.Id = "misc_nosky",
		.IsTimed = true
	}
);