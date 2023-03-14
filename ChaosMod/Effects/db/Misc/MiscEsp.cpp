#include <stdafx.h>

#include "Util/Graphics.h"
#include "Util/Colour.h"

static const float maxDistance = 75.f;
static const float lineThicknes = 0.001f;

static RGBA boxColour;

static bool WithinDistance(Ped from, Ped to)
{
	Vector3 fc = GET_ENTITY_COORDS(from, true);
	Vector3 tc = GET_ENTITY_COORDS(to, true);
 tc.z = fc.z;

	float dist = fc.DistanceTo(tc);
	return dist <= maxDistance && dist >= -maxDistance;
}

static void OnStart()
{
	boxColour = GetRandomColourRGB();
}

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		if (IS_ENTITY_ON_SCREEN(ped) && !IS_ENTITY_DEAD(ped, false) && !IS_PED_A_PLAYER(ped) && WithinDistance(playerPed, ped))
		{
			Vector3 coords = GET_PED_BONE_COORDS(ped, 0x796E, 0.f, 0.f, 0.f); // Head
			SET_DRAW_ORIGIN(coords.x, coords.y, coords.z, 0);
			Util::DRAW_2D_BOX(0, 0, 0.032, 0.073, boxColour, 0.001);
			CLEAR_DRAW_ORIGIN();
		}
	}
}

static RegisterEffect registerEffect(OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "ESP",
		.Id = "misc_esp",
		.IsTimed = true,
		.EffectGroupType = EEffectGroupType::Weapons
	}
);