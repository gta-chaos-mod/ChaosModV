/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

static const std::vector<const char*> weapons = { "WEAPON_PISTOL", "WEAPON_RAYPISTOL", "WEAPON_COMBATPISTOL", "WEAPON_APPISTOL", "WEAPON_PISTOL50", "WEAPON_STUNGUN", "WEAPON_SNSPISTOL", "WEAPON_HEAVYPISTOL", "WEAPON_REVOLVER", "WEAPON_MARKSMANPISTOL", "WEAPON_VINTAGEPISTOL", "WEAPON_FLAREGUN", "WEAPON_MICROSMG", "WEAPON_SMG", "WEAPON_ASSAULTSMG", "WEAPON_MACHINEPISTOL", "WEAPON_COMBATPDW", "WEAPON_PUMPSHOTGUN", "WEAPON_SAWNOFFSHOTGUN", "WEAPON_ASSAULTSHOTGUN", "WEAPON_BULLPUPSHOTGUN", "WEAPON_MUSKET", "WEAPON_HEAVYSHOTGUN", "WEAPON_DBSHOTGUN", "WEAPON_ASSAULTRIFLE", "WEAPON_CARBINERIFLE", "WEAPON_ADVANCEDRIFLE", "WEAPON_SPECIALCARBINE", "WEAPON_BULLPUPRIFLE", "WEAPON_COMPACTRIFLE", "WEAPON_MG", "WEAPON_COMBATMG", "WEAPON_GUSENBERG", "WEAPON_HEAVYSNIPER","WEAPON_MARKSMANRIFLE", "WEAPON_RPG", "WEAPON_RAILGUN", "WEAPON_MINIGUN" };

static void GenerateRandomLine(Vector3* a1, Vector3* a2)
{
	Ped plr = PLAYER_PED_ID();
	const float a1max = 10.f;
	const float a1min = -10.f;

	//Different for z coord, can't go -10 meters underground 
	const float a1zmax = 6.f;
	const float a1zmin = -0.5;

	Vector3 a1v = Vector3::Init
	(
		g_Random.GetRandomFloat(a1min, a1max),
		g_Random.GetRandomFloat(a1min, a1max),
		g_Random.GetRandomFloat(a1zmin, a1zmax)
	);
	*a1 = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(plr, a1v.x, a1v.y, a1v.z);

	//So we can get a straight line accross
	//x
	float a2xMin = (float)a1->x > 0 ? a1min : 0;
	float a2xMax = (float)a1->x > 0 ? 0 : a1max;
	//y
	float a2yMin = (float)a1->y > 0 ? a1min : 0;
	float a2yMax = (float)a1->y > 0 ? 0 : a1max;
	//z
	float a2zMin = (float)a1->z > 0 ? a1zmin : 0;
	float a2zMax = (float)a1->z > 0 ? 0 : a1zmax;
	Vector3 a2v = Vector3::Init
	(
		g_Random.GetRandomFloat(a2xMin, a2xMax),
		g_Random.GetRandomFloat(a2yMin, a2yMax),
		g_Random.GetRandomFloat(a2zMin, a2zMax)
	);
	*a2 = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(plr, a2v.x, a2v.y, a2v.z);
}

static void OnTick()
{
	static DWORD64 lastTick = 0;
	DWORD64 curTick = GET_GAME_TIMER();

	if (lastTick < curTick - 50)
	{
		lastTick = curTick;
		Vector3 from, to;
		GenerateRandomLine(&from, &to);
		Hash weapon = GET_HASH_KEY(weapons[g_Random.GetRandomInt(0, weapons.size()-1)]);
		SHOOT_SINGLE_BULLET_BETWEEN_COORDS(from.x, from.y, from.z, to.x, to.y, to.z, g_Random.GetRandomFloat(0, 100), false, weapon, 0, true, false, 50);
	}
}

static void OnStart()
{
	for (int i = 0; i < weapons.size(); i++)
	{
		REQUEST_WEAPON_ASSET(GET_HASH_KEY(weapons[i]), 31, 0); //Sometimes bullets may fail to materialize if the weapon asset isn't loaded, not my words
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_FLYING_BULLETS, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "John Wick",
		.Id = "misc_flying_bullets",
		.IsTimed = true,
		.IsShortDuration = true
	}
);