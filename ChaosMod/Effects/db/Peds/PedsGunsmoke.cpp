/*
    Effect by Last0xygen
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void OnTick()
{
	REQUEST_NAMED_PTFX_ASSET("scr_sr_tr");
	while (!HAS_NAMED_PTFX_ASSET_LOADED("scr_sr_tr"))
		WAIT(0);
	for (Ped ped : GetAllPeds())
	{
		if (IS_PED_SHOOTING(ped))
		{
			Vector3 pedPos = GET_ENTITY_COORDS(ped, false);
			USE_PARTICLE_FX_ASSET("scr_sr_tr");
			START_PARTICLE_FX_NON_LOOPED_AT_COORD("scr_sr_tr_car_change", pedPos.x, pedPos.y, pedPos.z, 0, 0, 0, 1,
			                                      false, true, false);
		}
	}
	REMOVE_NAMED_PTFX_ASSET("scr_sr_tr");
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, 
	{
		.Name = "Gunsmoke",
		.Id = "peds_gunsmoke",
		.IsTimed = true
	}
);