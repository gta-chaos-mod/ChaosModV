#include "stdafx.h"
#include "Util/Graphics.h"

static const float maxDistance = 100.f;
static const float lineThicknes = 0.001f;

static CRGBA RGBA;

static bool WithinDistance(Ped from, Ped to)
{
	Vector3 fc = GET_ENTITY_COORDS(from, true);
	Vector3 tc = GET_ENTITY_COORDS(to, true);

	bool x = tc.x - fc.x < maxDistance || fc.x - tc.x > -maxDistance;
	bool y = tc.y - fc.y < maxDistance || fc.y - tc.y > -maxDistance;
	return x && y;
}

static void OnStart()
{
	RGBA = g_Random.GetRandomColourRGB();
}

static void OnTick()
{
	static Ped playerPed = PLAYER_PED_ID();
	for (Ped ped : GetAllPeds())
	{
		if (IS_ENTITY_ON_SCREEN(ped) && !IS_ENTITY_DEAD(ped, false) && ped != playerPed && WithinDistance(playerPed, ped))
		{
			Vector3 coords = GET_PED_BONE_COORDS(ped, 0x796E, 0.f, 0.f, 0.f); // Head
			SET_DRAW_ORIGIN(coords.x, coords.y, coords.z, 0);
			Util::DRAW_2D_BOX(0, 0, 0.032, 0.073, RGBA, 0.001);
			CLEAR_DRAW_ORIGIN();
		}
	}
}

static RegisterEffect registerEffect(OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "ESP",
		.Id = "misc_esp",
		.IsTimed = true,
		.IsShortDuration = true,
		.EffectGroupType = EEffectGroupType::Weapons
	}
);