//
// effect by veloscocity
//

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

#define PI 3.14159265

CHAOS_VAR float x, y;

CHAOS_VAR DWORD lastTick = 0;
static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();
	float rot =
	    GET_ENTITY_HEADING(IS_PED_IN_ANY_VEHICLE(playerPed, false) ? GET_VEHICLE_PED_IS_IN(playerPed, false)
	                                                               : playerPed); // Shake perpendicular to the player
	x = sin((360 - rot) * PI / 180) * 1.33f;
	y = -cos((360 - rot) * PI / 180) * 1.33f;
}
static void OnTick()
{
	DWORD curTick = GetTickCount();
	if (lastTick < curTick - 500)
	{
		lastTick = curTick;
		x        = -x;
		y        = -y;
	}

	for (auto object : GetAllProps())
		Memory::ApplyForceToEntityCenterOfMass(object, 1, y, x, 0, false, false, true, false);

	for (auto veh : GetAllVehs())
		Memory::ApplyForceToEntityCenterOfMass(veh, 1, y, x, 0, false, false, true, false);
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, OnTick, 
	{
		.Name = "Cocktail Shaker",
		.Id = "cocktail_shaker",
		.IsTimed = true,
		.IsShortDuration = true,
		.EffectCategory = EffectCategory::Gravity,
	}
);
