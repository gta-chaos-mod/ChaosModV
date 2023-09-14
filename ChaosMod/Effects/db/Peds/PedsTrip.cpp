#include <stdafx.h>
#include "Memory/Physics.h"
#include "Util/Random.h"

#define PI 3.14159265

static void OnStart()
{
	std::list<Ped> peds;
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_IN_ANY_VEHICLE(ped, false))
			peds.push_back(ped);
        Hash vehModel = GET_ENTITY_MODEL(GET_VEHICLE_PED_IS_IN(ped, false));
		if (IS_THIS_MODEL_A_BIKE(vehModel)
		    || IS_THIS_MODEL_A_BICYCLE(vehModel))
			peds.push_back(ped);
	}

    for(Ped ped : peds)
		SET_PED_TO_RAGDOLL(ped, 4000, 4000, 0, true, true, false);
    
    WAIT(0);
    for(Ped ped : peds)
    {
        float rot = GET_ENTITY_HEADING(ped);
        auto x = sin((360 - rot) * PI / 180);
        auto y = cos((360 - rot) * PI / 180);
        float force = g_Random.GetGammaRandomFloat(2.4f, 2.1f) * 2750.f;
		Memory::ApplyForceToEntityCenterOfMass(ped, 0, x * force, y * force, 0.f, false, false, true, false);
    }
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Trip Everyone",
		.Id = "peds_trip"
	}
);