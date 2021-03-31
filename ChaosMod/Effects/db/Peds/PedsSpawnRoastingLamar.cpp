#include <stdafx.h>

/*
	Effect by Juhana
*/

static Ped lamarPed;

static void OnStart()
{
	static const Hash lamarModel = GET_HASH_KEY("ig_lamardavis");

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_ROASTING_LAMAR", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, GET_HASH_KEY("PLAYER"));
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, GET_HASH_KEY("PLAYER"), relationshipGroup);

	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	LoadModel(lamarModel);
	lamarPed = CREATE_PED(4, lamarModel, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(playerPed), true, false);
	SET_MODEL_AS_NO_LONGER_NEEDED(lamarModel);

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		SET_PED_INTO_VEHICLE(lamarPed, GET_VEHICLE_PED_IS_IN(playerPed, false), -2);
	}

	SET_PED_RELATIONSHIP_GROUP_HASH(lamarPed, relationshipGroup);
	SET_PED_AS_GROUP_MEMBER(lamarPed, GET_PLAYER_GROUP(PLAYER_ID()));
	SET_ENTITY_INVINCIBLE(lamarPed, true);

	_PLAY_AMBIENT_SPEECH1(lamarPed, "GENERIC_HI", "SPEECH_PARAMS_FORCE_SHOUTED", 1);
	WAIT(1500);
	_PLAY_AMBIENT_SPEECH1(playerPed, "GENERIC_FUCK_YOU", "SPEECH_PARAMS_FORCE_SHOUTED", 1);
	WAIT(2500);
}

static void OnStop()
{
	Ped playerPed = PLAYER_PED_ID();

	if (DOES_ENTITY_EXIST(lamarPed))
	{
		if (!IS_PED_DEAD_OR_DYING(lamarPed, true))
		{
			REQUEST_ANIM_DICT("mp_player_int_upperfinger");

			TASK_TURN_PED_TO_FACE_ENTITY(lamarPed, playerPed, 1000);
			TASK_LOOK_AT_ENTITY(lamarPed, playerPed, 1000, 2048, 3);
			WAIT(1000);

			TASK_PLAY_ANIM(lamarPed, "mp_player_int_upperfinger", "mp_player_int_finger_02", 8.0f, -1.0f, 1000.f, 1, 0.f, false, false, false);
			WAIT(2000);
			_PLAY_AMBIENT_SPEECH1(playerPed, "GENERIC_SHOCKED_MED", "SPEECH_PARAMS_FORCE_SHOUTED", 1);
			
			if (IS_PED_IN_ANY_VEHICLE(lamarPed, false))
			{
				Vehicle veh = GET_VEHICLE_PED_IS_IN(lamarPed, false);
				TASK_LEAVE_VEHICLE(lamarPed, veh, 4160);
			}
		}

		SET_PED_AS_NO_LONGER_NEEDED(&lamarPed);
		SET_ENTITY_INVINCIBLE(lamarPed, false);
	}
}

static void OnTick()
{
	static DWORD64 lastTick = 0;
	DWORD64 curTick = GET_GAME_TIMER();
	Ped playerPed = PLAYER_PED_ID();

	if (lastTick < curTick - 500)
	{
		lastTick = curTick;

		if (DOES_ENTITY_EXIST(lamarPed))
		{
			_PLAY_AMBIENT_SPEECH1(lamarPed, "GENERIC_INSULT_MED", "SPEECH_PARAMS_FORCE_SHOUTED", 1);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PEDS_ROASTING, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Get Roasted",
		.Id = "peds_roasting",
		.IsTimed = true,
		.IsShortDuration = true
	}
);