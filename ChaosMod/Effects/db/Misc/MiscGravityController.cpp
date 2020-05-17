#include <stdafx.h>

static void OnStop()
{
	SET_GRAVITY_LEVEL(0);
}

static void OnTickLow()
{
	SET_GRAVITY_LEVEL(1);
}

static RegisterEffect registerEffect1(EFFECT_LOW_GRAV, nullptr, OnStop, OnTickLow);

static void OnTickVeryLow()
{
	SET_GRAVITY_LEVEL(2);
}

static RegisterEffect registerEffect2(EFFECT_VERY_LOW_GRAV, nullptr, OnStop, OnTickVeryLow);

static void OnTickInsane()
{
	Memory::SetGravityLevel(200.f);

	for (auto ped : GetAllPeds())
	{
		if (!IS_PED_IN_ANY_VEHICLE(ped, false))
		{
			SET_PED_TO_RAGDOLL(ped, 1000, 1000, 0, true, true, false);

			APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(ped, 0, 0, 0, -75.f, false, false, true, false);
		}
	}

	for (auto object : GetAllProps())
	{
		APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(object, 0, 0, 0, -200.f, false, false, true, false);
	}
}

static RegisterEffect registerEffect3(EFFECT_INSANE_GRAV, nullptr, OnStop, OnTickInsane);

static void OnStartInvert()
{
	GIVE_WEAPON_TO_PED(PLAYER_PED_ID(), GET_HASH_KEY("WEAPON_PARACHUTE"), 9999, true, true);
}

static void OnTickInvert()
{
	Memory::SetGravityLevel(-1.f);

	for (auto ped : GetAllPeds())
	{
		if (!IS_PED_IN_ANY_VEHICLE(ped, false))
		{
			SET_PED_TO_RAGDOLL(ped, 1000, 1000, 0, true, true, false);

			APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(ped, 0, 0, 0, 25.f, false, false, true, false);
		}
	}

	for (auto object : GetAllProps())
	{
		APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(object, 0, 0, 0, 100.f, false, false, true, false);
	}
}

static RegisterEffect registerEffect4(EFFECT_INVERT_GRAV, OnStartInvert, OnStop, OnTickInvert);