#include "stdafx.h"

#define NULL ((void*)0)

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
	EEffectType type;
	Hash model;
	FakeSpawnType spawnType;
	const PedSpawnAttributes* pedAttributes = nullptr;
};

static const std::vector<FakeSpawnInfo> fakeSpawns =
{
	{EFFECT_SPAWN_ADDER, 0xB779A091, FakeSpawnType::VEHICLETYPE}, // Adder
	{EFFECT_ANGRY_ALIEN, 0x570462B9, FakeSpawnType::PEDTYPE, new PedSpawnAttributes(0x7FD62962, 5)}, // Jimmy
	{EFFECT_SPAWN_TANK, 0x2EA68690, FakeSpawnType::VEHICLETYPE}, // Rhino
	{EFFECT_ANGRY_JESUS, 0xCE2CB751, FakeSpawnType::PEDTYPE, new PedSpawnAttributes(0x6D544C99, 5)}, // Griefer Jesus
	{EFFECT_SPAWN_BUZZARD, 0x2F03547B, FakeSpawnType::VEHICLETYPE}, // Buzzard
	{EFFECT_SPAWN_IMPOTENTRAGE, 0x348065F5, FakeSpawnType::PEDTYPE, new PedSpawnAttributes(0, 5)}, // Impotent Rage
	{EFFECT_SPAWN_MONSTER, 0xCD93A7DB, FakeSpawnType::VEHICLETYPE}, // Monster
	{EFFECT_SPAWN_COMPANION_BRAD, 0xBDBB4922, FakeSpawnType::PEDTYPE, new PedSpawnAttributes(0xB1CA77B1, 0)}, // Brad (Companion)
};

static void OnStart()
{
	FakeSpawnInfo selectedSpawnInfo = fakeSpawns.at(g_Random.GetRandomInt(0, fakeSpawns.size()-1));
	EEffectType overideType = selectedSpawnInfo.type;
	Hash modelHash = selectedSpawnInfo.model;
	GetComponent<EffectDispatcher>()->OverrideEffectName(EFFECT_FAKE_SPAWN, overideType);

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

static RegisterEffect registerEffect(EFFECT_FAKE_SPAWN, OnStart, EffectInfo
	{
		.Name = "Fake Spawn",
		.Id = "misc_fake_spawn",
		.EffectGroupType = EEffectGroupType::SpawnGeneric
	}
);