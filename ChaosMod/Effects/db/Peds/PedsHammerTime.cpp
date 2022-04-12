#include "stdafx.h"
#include <list>

Hash ped = GET_HASH_KEY("S_M_Y_Construct_02");
Hash hammer = 1317494643;
Hash relationshipGroupPeds;

static float waitTime = 2;
bool playerHasHammer = false;

std::list<Ped> _peds;


static void OnTick()
{
	static const Hash riotGroupHash = GET_HASH_KEY("_HAMMERTIME");
	static const Hash playerGroupHash = GET_HASH_KEY("PLAYER");

	SET_RELATIONSHIP_BETWEEN_GROUPS(5, riotGroupHash, riotGroupHash);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, riotGroupHash, playerGroupHash);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, playerGroupHash, riotGroupHash);

	SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), 0, false);
	SET_MAX_WANTED_LEVEL(0);

	static std::list<Ped> goneThroughPeds;

	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			SET_PED_RELATIONSHIP_GROUP_HASH(ped, riotGroupHash);

			SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
			SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);

			SET_PED_FIRING_PATTERN(ped, 0xC6EE6B4C);

			if (std::find(goneThroughPeds.begin(), goneThroughPeds.end(), ped) == goneThroughPeds.end())
			{
				static const std::vector<Hash>& weps = Memory::GetAllWeapons();
				GIVE_WEAPON_TO_PED(ped, hammer, 9999, false, true);

				goneThroughPeds.push_back(ped);
			}
		}
	}

	for (std::list<Ped>::iterator it = goneThroughPeds.begin(); it != goneThroughPeds.end(); )
	{
		if (!DOES_ENTITY_EXIST(*it))
		{
			it = goneThroughPeds.erase(it);
		}
		else
		{
			it++;
		}
	}

	DISABLE_CONTROL_ACTION(1, 37, 1);
}

static void OnStop()
{	
	REMOVE_RELATIONSHIP_GROUP(GET_HASH_KEY("_HAMMERTIME"));
	static Hash resetGroupHash;
	ADD_RELATIONSHIP_GROUP("_HAMMERTIME_RESET", &resetGroupHash);
	static const Hash playerGroupHash = GET_HASH_KEY("PLAYER");

	SET_RELATIONSHIP_BETWEEN_GROUPS(255, resetGroupHash, resetGroupHash);
	SET_RELATIONSHIP_BETWEEN_GROUPS(255, resetGroupHash, playerGroupHash);
	SET_RELATIONSHIP_BETWEEN_GROUPS(255, playerGroupHash, resetGroupHash);

	for (Ped p : GetAllPeds())
	{
		if (p == PLAYER_PED_ID() && playerHasHammer == false)
		{
			REMOVE_WEAPON_FROM_PED(p, hammer);
			continue;
		}
		REMOVE_WEAPON_FROM_PED(p, hammer);
		SET_PED_RELATIONSHIP_GROUP_HASH(p, resetGroupHash);
	}

	//Give player weapon wheel back
	DISABLE_CONTROL_ACTION(1, 37, 0);
}

static void OnStart()
{

	Hash groupHash;
	ADD_RELATIONSHIP_GROUP("_HAMMERTIME", &groupHash);

	Ped playerPed = PLAYER_PED_ID();

	if (HAS_PED_GOT_WEAPON(playerPed, hammer, 0))
	{
		playerHasHammer = true;
	} 
	else
	{
		GIVE_WEAPON_TO_PED(playerPed, hammer, 9999, false, true);
	}

	for (auto ped : GetAllPeds())
	{
		if (IS_PED_IN_ANY_VEHICLE(ped, true))
		{
			Vehicle pedVeh = GET_VEHICLE_PED_IS_IN(ped, true);
			TASK_LEAVE_VEHICLE(ped, pedVeh, 256);
			BRING_VEHICLE_TO_HALT(pedVeh, 0.1f, 10, 0);
		}
		if (ped != playerPed)
		{
			TASK_TURN_PED_TO_FACE_ENTITY(ped, playerPed, -1);
			TASK_LOOK_AT_ENTITY(ped, playerPed, -1, 2048, 3);
		}
	}

	Vector3 pos = GET_ENTITY_COORDS(playerPed, 1);
	Ped enemy = CreatePoolPed(4, ped, pos.x, pos.y, pos.z, 0.f);
	WAIT((DWORD)waitTime*1000);
}

static RegisterEffect reg(EFFECT_PEDS_HAMMERTIME, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Stop! Hammer Time",
		.Id = "peds_hammertime",
		.IsTimed = true,
		.IsShortDuration = false
	}
);