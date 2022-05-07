#include <stdafx.h>

//effect by ubscal, modified from Bus Bois, modified by Last0xygen

static void OnStart()
{
	static const Hash mowerHash = GET_HASH_KEY("MOWER");
	SetSurroundingPedsInVehicles(mowerHash, 120);
}

static RegisterEffect registerEffect(EFFECT_PEDS_MOWERMATES, OnStart, EffectInfo
	{
		.Name = "Mower Mates",
		.Id = "peds_mowermates"
	}
);
