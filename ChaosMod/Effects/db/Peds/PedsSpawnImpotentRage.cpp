#include <stdafx.h>

static void OnStart()
{
	static const Hash playerGroup = GET_HASH_KEY("PLAYER");
	static const Hash civGroup    = GET_HASH_KEY("CIVMALE");
	static const Hash femCivGroup = GET_HASH_KEY("CIVFEMALE");

	Hash relGroup;
	ADD_RELATIONSHIP_GROUP("_IM_RAGE", &relGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relGroup, playerGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relGroup, civGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(5, relGroup, femCivGroup);

	Hash model        = GET_HASH_KEY("u_m_y_imporage");

	Ped playerPed     = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	Ped ped           = CreatePoolPed(4, model, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(playerPed));
	SET_ENTITY_HEALTH(ped, 1000, 0);
	SET_PED_ARMOUR(ped, 1000);

	SET_PED_RELATIONSHIP_GROUP_HASH(ped, relGroup);
	SET_PED_HEARING_RANGE(ped, 9999.f);
	SET_PED_CONFIG_FLAG(ped, 281, true);
	SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
	SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);
	SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(ped, false);
	SET_RAGDOLL_BLOCKING_FLAGS(ped, 5);
	SET_PED_SUFFERS_CRITICAL_HITS(ped, false);

	TASK_COMBAT_PED(ped, playerPed, 0, 16);

	SET_PED_FIRING_PATTERN(ped, 0xC6EE6B4C);

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		SET_PED_INTO_VEHICLE(ped, GET_VEHICLE_PED_IS_IN(playerPed, false), -2);
	}

	DWORD64 lastTick = GET_GAME_TIMER();

	while (!REQUEST_SCRIPT_AUDIO_BANK("DLC_VINEWOOD/DLC_VW_HIDDEN_COLLECTIBLES", true, 0))
	{
		DWORD64 curTick = GET_GAME_TIMER();

		if (lastTick < curTick - 500)
		{
			break;
		}

		WAIT(0);
	}

	for (int i = 0; i < 3; i++)
	{
		PLAY_SOUND_FRONTEND(-1, "impotent_rage", "dlc_vw_hidden_collectible_sounds", false);
	}

	RELEASE_NAMED_SCRIPT_AUDIO_BANK("DLC_VINEWOOD/DLC_VW_HIDDEN_COLLECTIBLES");
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Spawn Impotent Rage",
		.Id = "peds_spawnimrage",
		.EffectGroupType = EEffectGroupType::SpawnEnemySpecial
	}
);