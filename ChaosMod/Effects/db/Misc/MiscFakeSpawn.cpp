#include "stdafx.h"

#include "Components/Component.h"
#include "Components/EffectDispatcher.h"
#include "Memory/Hooks/ScriptThreadRunHook.h"

static enum class FakeSpawnType : int
{
	PEDTYPE, 
	VEHICLETYPE
};

struct PedSpawnAttributes
{
	int weapon = 0;
	int relationship = 255;

	PedSpawnAttributes(int _weapon, int _relation)
		: weapon(_weapon), relationship(_relation)
	{
	}
};

struct FakeSpawnInfo
{
	const char* id;
	Hash model;
	FakeSpawnType spawnType;
	PedSpawnAttributes* pedAttributes = nullptr;
};

static const std::vector<FakeSpawnInfo> fakeSpawns =
{
	{"spawn_adder", 0xB779A091, FakeSpawnType::VEHICLETYPE}, // Adder
	{"peds_angryjimmy", 0x570462B9, FakeSpawnType::PEDTYPE, new PedSpawnAttributes(0x7FD62962, 5)}, // Jimmy
	{"spawn_rhino", 0x2EA68690, FakeSpawnType::VEHICLETYPE}, // Rhino
	{"spawn_grieferjesus", 0xCE2CB751, FakeSpawnType::PEDTYPE, new PedSpawnAttributes(0x6D544C99, 5)}, // Griefer Jesus
	{"spawn_buzzard", 0x2F03547B, FakeSpawnType::VEHICLETYPE}, // Buzzard
	{"peds_spawnimrage", 0x348065F5, FakeSpawnType::PEDTYPE, new PedSpawnAttributes(0, 5)}, // Impotent Rage
	{"spawn_monster", 0xCD93A7DB, FakeSpawnType::VEHICLETYPE}, // Monster
	{"spawn_compbrad", 0xBDBB4922, FakeSpawnType::PEDTYPE, new PedSpawnAttributes(0xB1CA77B1, 0)}, // Brad (Companion)
};

static void OnStart()
{
	FakeSpawnInfo selectedSpawnInfo = fakeSpawns.at(g_Random.GetRandomInt(0, fakeSpawns.size()-1));
	const char* overideId = selectedSpawnInfo.id;
	Hash modelHash = selectedSpawnInfo.model;
	
	GetComponent<EffectDispatcher>()->OverrideEffectNameId("misc_fake_spawn", overideId);

	LoadModel(modelHash);

	Hooks::EnableScriptThreadBlock();

	Ped plrPed = PLAYER_PED_ID();
	Vector3 spawnPos = GET_ENTITY_COORDS(plrPed, true);
	Entity spawnedEnt;

	switch (selectedSpawnInfo.spawnType)
	{
	case FakeSpawnType::PEDTYPE:
	{
		spawnedEnt = CreatePoolPed(4, modelHash, spawnPos.x, spawnPos.y, spawnPos.z, GET_ENTITY_HEADING(plrPed));
		if (selectedSpawnInfo.pedAttributes)
		{
			PedSpawnAttributes psa = *selectedSpawnInfo.pedAttributes;
			Hash relationshipGroup;
			ADD_RELATIONSHIP_GROUP("_FAKE_SPAWN", &relationshipGroup);
			SET_RELATIONSHIP_BETWEEN_GROUPS(psa.relationship, relationshipGroup, GET_HASH_KEY("PLAYER"));

			SET_PED_RELATIONSHIP_GROUP_HASH(spawnedEnt, relationshipGroup);

			GIVE_WEAPON_TO_PED(spawnedEnt, psa.weapon, 9999, true, true);

			if (psa.relationship == 5)
			{
				TASK_COMBAT_PED(spawnedEnt, plrPed, 0, 16);
			}

			SET_PED_FIRING_PATTERN(spawnedEnt, 0xC6EE6B4C);
		}
		break;
	}
	case FakeSpawnType::VEHICLETYPE:
		spawnedEnt = CreatePoolVehicle(modelHash, spawnPos.x, spawnPos.y, spawnPos.z, GET_ENTITY_HEADING(plrPed));
		break;
	}

	WAIT(g_Random.GetRandomInt(4000, 8000));

	DELETE_ENTITY(&spawnedEnt);

	WAIT(0);

	UnloadModel(modelHash);

	Hooks::DisableScriptThreadBlock();
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Fake Spawn",
		.Id = "misc_fake_spawn",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
	}
);
