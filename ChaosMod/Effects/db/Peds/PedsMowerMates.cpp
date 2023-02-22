// Effect by Lunascape, modified from Bus Bois, modified by Last0xygen

#include <stdafx.h>

#include "Util/Vehicle.h"

static void OnStart()
{
	static const Hash mowerHash = "MOWER"_hash;
	SetSurroundingPedsInVehicles(mowerHash, 120);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Mower Mates",
		.Id = "peds_mowermates"
	}
);
