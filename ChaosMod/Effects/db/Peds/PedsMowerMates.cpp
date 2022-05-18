// Effect by ubscal, modified from Bus Bois, modified by Last0xygen

#include <stdafx.h>

#include "Util/Vehicle.h"

static void OnStart()
{
	static const Hash mowerHash = GET_HASH_KEY("MOWER");
	SetSurroundingPedsInVehicles(mowerHash, 120);
}

static RegisterEffect registerEffect(OnStart, EffectInfo
	{
		.Name = "Mower Mates",
		.Id = "peds_mowermates"
	}
);
