#include <stdafx.h>

#include "Components/EffectDispatcher.h"
#include "Memory/Hooks/ScriptThreadRunHook.h"

static void OnStartFive()
{
	Player player = PLAYER_ID();

	SET_PLAYER_WANTED_LEVEL(player, 5, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
}

// clang-format off
REGISTER_EFFECT(OnStartFive, nullptr, nullptr, EffectInfo
	{
		.Name = "5 Wanted Stars",
		.Id = "player_5stars",
		.IncompatibleWith = { "player_neverwanted" },
                .EffectGroupType = EffectGroupType::WantedLevel
	}
);
// clang-format on

static void OnStartPlusTwo()
{
	Ped player = PLAYER_ID();

	SET_PLAYER_WANTED_LEVEL(player, PLAYER::GET_PLAYER_WANTED_LEVEL(player) + 2, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
}

// clang-format off
REGISTER_EFFECT(OnStartPlusTwo, nullptr, nullptr, EffectInfo
	{
		.Name = "+2 Wanted Stars",
		.Id = "player_plus2stars",
		.IncompatibleWith = { "player_neverwanted" },
                .EffectGroupType = EffectGroupType::WantedLevel
	}
);
// clang-format on

static void OnTickNeverWanted()
{
	SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), 0, false);
	SET_PLAYER_WANTED_LEVEL_NOW(PLAYER_ID(), true);
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTickNeverWanted, EffectInfo
	{
		.Name = "Never Wanted",
		.Id = "player_neverwanted",
		.IsTimed = true,
                .EffectGroupType = EffectGroupType::WantedLevel
	}
);
// clang-format on

static void OnStartThree()
{
	Player player = PLAYER_ID();

	SET_PLAYER_WANTED_LEVEL(player, 3, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
}

// clang-format off
REGISTER_EFFECT(OnStartThree, nullptr, nullptr, EffectInfo
	{
		.Name = "3 Wanted Stars",
		.Id = "player_3stars",
		.IncompatibleWith = { "player_neverwanted" },
                .EffectGroupType = EffectGroupType::WantedLevel
	}
);
// clang-format on

static void OnStartOne()
{
	Player player = PLAYER_ID();

	SET_PLAYER_WANTED_LEVEL(player, 1, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
}

// clang-format off
REGISTER_EFFECT(OnStartOne, nullptr, nullptr, EffectInfo
	{
		.Name = "1 Wanted Star",
		.Id = "player_1star",
		.IncompatibleWith = { "player_neverwanted" },
                .EffectGroupType = EffectGroupType::WantedLevel
	}
);
// clang-format on

enum WantedLevelType
{
	SET,
	ADD
};

struct FakeWantedInfo
{
	std::string Name;
	WantedLevelType WantedType;
	int Stars;
};

static std::vector<FakeWantedInfo> ms_FakeWantedLevels = {
	{ "5 Wanted Stars", SET, 5 },
	{ "3 Wanted Stars", SET, 3 },
	{ "1 Wanted Star", SET, 1 },
	{ "+2 Wanted Stars", ADD, 2 },
};

int lastLevel;
int lastLevelf;

static void OnStartFake()
{
	FakeWantedInfo selectedInfo  = ms_FakeWantedLevels.at(g_Random.GetRandomInt(0, ms_FakeWantedLevels.size() - 1));
	WantedLevelType selectedType = selectedInfo.WantedType;

	Hooks::EnableScriptThreadBlock();

	if (ComponentExists<EffectDispatcher>())
	{
		GetComponent<EffectDispatcher>()->OverrideEffectName("player_fakestars", selectedInfo.Name);
	}

	Player player  = PLAYER_ID();
	lastLevel  = GET_PLAYER_WANTED_LEVEL(player);
	lastLevelf = GET_FAKE_WANTED_LEVEL();

	switch (selectedType)
	{
	case SET:
	{
		SET_FAKE_WANTED_LEVEL(selectedInfo.Stars);
		break;
	}
	case ADD:
	{
		int wl = [&]()
		{
			int l = selectedInfo.Stars + (lastLevelf == 0 ? lastLevel : lastLevelf);
			if (l > 5)
				l = 5;
			return l;
		}();
		SET_FAKE_WANTED_LEVEL(wl);
		break;
	}
	}

	WAIT(g_Random.GetRandomInt(2500, 6500));

	Hooks::DisableScriptThreadBlock();
	SET_FAKE_WANTED_LEVEL(lastLevelf);
}

// clang-format off
REGISTER_EFFECT(OnStartFake, nullptr, nullptr, EffectInfo
	{
		.Name = "Fake Wanted Level",
		.Id = "player_fakestars",
		.HideRealNameOnStart = true,
		.IncompatibleWith = { "player_neverwanted" },
        .EffectGroupType = EffectGroupType::WantedLevel
	}
);

static void spawnMesa()
{
	Ped playerPed     = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);
	Vector3 spawnPoint;
	// Try spawning on a vehicle node, fall back to random coord
	int nodeId;
	if (!GET_RANDOM_VEHICLE_NODE(playerPos.x, playerPos.y, playerPos.z, 150, false, false, false, &spawnPoint, &nodeId))
	{
		spawnPoint = getRandomOffsetCoord(playerPos, 50, 50);
		float groundZ;
		if (GET_GROUND_Z_FOR_3D_COORD(spawnPoint.x, spawnPoint.y, spawnPoint.z, &groundZ, false, false))
		{
			spawnPoint.z = groundZ;
		}
	}
	float xDiff   = playerPos.x - spawnPoint.x;
	float yDiff   = playerPos.y - spawnPoint.y;
	float heading = GET_HEADING_FROM_VECTOR_2D(xDiff, yDiff);
	Hash mesaHash = "Mesa3"_hash;
	mesaGroup     = EnemyGroup();
	LoadModel(mesaHash);
	mesaGroup.vehicle =
	    CREATE_VEHICLE(mesaHash, spawnPoint.x, spawnPoint.y, spawnPoint.z + 5, heading, true, false, false);
	SET_VEHICLE_ON_GROUND_PROPERLY(mesaGroup.vehicle, 5);
	SET_VEHICLE_COLOURS(mesaGroup.vehicle, 0, 0);
	SET_VEHICLE_ENGINE_ON(mesaGroup.vehicle, true, true, true);
	SET_VEHICLE_CHEAT_POWER_INCREASE(mesaGroup.vehicle, 2); // Make it easier to catch up
	fillVehicleWithPeds(mesaGroup.vehicle, playerPed, relationshipGroup, model, microSmgHash, mesaGroup.peds, true);
}

static void OnStartSix(void)
{

}

// clang-format off
REGISTER_EFFECT(OnStartSix, nullptr, nullptr, EffectInfo
	{
		.Name = "!6 Wanted Stars",
		.Id = "player_6stars",
		.IncompatibleWith = { "player_neverwanted" },
        .EffectGroupType = EffectGroupType::WantedLevel,
		//.IsTimed = true
	}
);
