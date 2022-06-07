/*
	Effect By OnlyRealNubs
*/

#include <stdafx.h>

#include "Util/Peds.h"

static std::list<Ped> m_aliens;
static const std::vector<Hash> m_weapons = 
{ 
	GET_HASH_KEY("WEAPON_RAYPISTOL"),
	GET_HASH_KEY("WEAPON_RAYCARBINE"),
	GET_HASH_KEY("WEAPON_RAYMINIGUN")
};

#pragma region UFO 
static const Hash ufoModel = 0xB467C540;
static Object ufo;
static bool ufoBusy     = false;
#pragma endregion

#pragma region PTFX
static const char *ptfxDict = "scr_rcbarry1";
static const char *ptfxCreate = "scr_alien_teleport";
#pragma endregion

static void OnStart()
{
	m_aliens.clear();
	Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), true);

	LoadModel(ufoModel);
	ufo = CreatePoolProp(ufoModel, 0.f, -4000.f, playerPos.z + 40, true);
	SET_ENTITY_AS_MISSION_ENTITY(ufo, false, false);
	SET_MODEL_AS_NO_LONGER_NEEDED(ufoModel);

	REQUEST_NAMED_PTFX_ASSET(ptfxDict);
	while (!HAS_NAMED_PTFX_ASSET_LOADED(ptfxDict))
	{
		WAIT(0);
	}

	Hash groupHash;
	ADD_RELATIONSHIP_GROUP("_ALIENS", &groupHash);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, groupHash, GET_HASH_KEY("PLAYER"));
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, groupHash, GET_HASH_KEY("CIVMALE"));
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, groupHash, GET_HASH_KEY("CIVFEMALE"));

	ENABLE_ALIEN_BLOOD_VFX(true);

	SET_AI_MELEE_WEAPON_DAMAGE_MODIFIER(.1f);
	SET_AI_WEAPON_DAMAGE_MODIFIER(.1f);

	while (!SLIDE_OBJECT(ufo, playerPos.x, playerPos.y, GET_ENTITY_COORDS(ufo, true).z, 10.f, 10.f, 1.f, false))
	{
		WAIT(0);
	}
}

static void OnStop()
{
	for (Ped ped : m_aliens)
	{
		if (DOES_ENTITY_EXIST(ped))
		{
			Vector3 pedPos = GET_ENTITY_COORDS(ped, true);
			USE_PARTICLE_FX_ASSET(ptfxDict);
			START_PARTICLE_FX_NON_LOOPED_AT_COORD(ptfxCreate, pedPos.x, pedPos.y, pedPos.z, 0.f, 0.f, 0.f, 1.2f,
			                                      false, false, false);
			DELETE_PED(&ped);
		}
	}
	ufoBusy = false;

	while (!SLIDE_OBJECT(ufo, 0.f, -4000.f, GET_ENTITY_COORDS(ufo, true).z, 10.f, 10.f, 1.f, false))
	{
		WAIT(0);
	}
	
	if (DOES_ENTITY_EXIST(ufo))
	{
		SET_OBJECT_AS_NO_LONGER_NEEDED(&ufo);
	}

	ENABLE_ALIEN_BLOOD_VFX(false);

	RESET_AI_MELEE_WEAPON_DAMAGE_MODIFIER();
	RESET_AI_WEAPON_DAMAGE_MODIFIER();
}

static void OnTick()
{
	static DWORD64 lastTick;
	DWORD64 curTick                  = GetTickCount64();
	static constexpr int MAX_ALIENS = 5;
	static constexpr Hash MODEL_HASH = 0x64611296;

	static Hash alienGroupHash      = GET_HASH_KEY("_ALIENS");

	Ped playerPed                    = PLAYER_PED_ID();
	Vector3 playerPos                = GET_ENTITY_COORDS(playerPed, false);

	if (m_aliens.size() <= MAX_ALIENS && !ufoBusy)
	{
		Vector3 spawnPos;
		if (GET_NTH_CLOSEST_VEHICLE_NODE(playerPos.x, playerPos.y, playerPos.z, 15 + m_aliens.size(), &spawnPos, 0, 0,
		                                 0)
		    && GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, spawnPos.x, spawnPos.y, spawnPos.z,
		                                   false)
		           < 300.f)
		{
			ufoBusy = true;

			while (!SLIDE_OBJECT(ufo, spawnPos.x, spawnPos.y, GET_ENTITY_COORDS(ufo, true).z, 1.f, 1.f, 1.f, false))
			{
				WAIT(0);
			}

			LoadModel(MODEL_HASH);

			Ped alien = CreatePoolPed(26, MODEL_HASH, spawnPos.x, spawnPos.y, spawnPos.z, .0f);

			SET_PED_COMPONENT_VARIATION(alien, 0, 0, 0, 0);
			SET_PED_COMPONENT_VARIATION(alien, 3, 0, 0, 0);
			SET_PED_COMPONENT_VARIATION(alien, 4, 0, 0, 0);
			SET_PED_COMPONENT_VARIATION(alien, 5, 0, 0, 0);
			SET_PED_COMPONENT_VARIATION(alien, 6, 0, 0, 0);

			m_aliens.push_back(alien);

			SET_PED_RELATIONSHIP_GROUP_HASH(alien, alienGroupHash);
			SET_PED_COMBAT_ATTRIBUTES(alien, 5, true);
			SET_PED_COMBAT_ATTRIBUTES(alien, 46, true);

			SET_AMBIENT_VOICE_NAME(alien, "ALIENS");
			DISABLE_PED_PAIN_AUDIO(alien, true);

			GIVE_WEAPON_TO_PED(alien, m_weapons.at(g_Random.GetRandomInt(0, m_weapons.size()-1)), 9999, false, true);

			TASK_COMBAT_PED(alien, playerPed, 0, 16);
			SET_PED_FIRING_PATTERN(alien, 0x7A845691);

			SET_MODEL_AS_NO_LONGER_NEEDED(MODEL_HASH);

			Vector3 pedPos = GET_ENTITY_COORDS(alien, true);
			USE_PARTICLE_FX_ASSET(ptfxDict);
			START_PARTICLE_FX_NON_LOOPED_AT_COORD(ptfxCreate, pedPos.x, pedPos.y, pedPos.z, 0.f, 0.f, 0.f, 1.2f, false,
			                                      false, false);

			ufoBusy = false;
		}
	}

	for (std::list<Ped>::iterator it = m_aliens.begin(); it != m_aliens.end();)
	{
		Ped alien = *it;
		if (DOES_ENTITY_EXIST(alien))
		{
			Vector3 alienPos = GET_ENTITY_COORDS(alien, false);
			if (GET_DISTANCE_BETWEEN_COORDS(playerPos.x, playerPos.y, playerPos.z, alienPos.x, alienPos.y, alienPos.z,
			                                false)
			    < 300.f)
			{
				if (IS_PED_DEAD_OR_DYING(alien, true))
				{
					SET_PED_AS_NO_LONGER_NEEDED(&alien);
				}
				it++;
				continue;
			}
			SET_PED_AS_NO_LONGER_NEEDED(&alien);
		}
		it = m_aliens.erase(it);
	}

	//Slowly rotate the ufo
	if (lastTick < curTick - 10)
	{
		lastTick = curTick;

		SET_ENTITY_ROTATION(ufo, 0.f, 0.f, GET_ENTITY_HEADING(ufo) + 0.1f, 2, true);
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Alien Invasion",
		.Id = "aliens",
		.IsTimed = true
	}
);