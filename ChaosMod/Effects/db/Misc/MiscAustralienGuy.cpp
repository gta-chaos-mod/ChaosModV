#include <stdafx.h>

static void OnStop()
{
	SET_MODEL_AS_NO_LONGER_NEEDED(GET_HASH_KEY("prop_ecola_can"));

	SET_PLAYER_HEALTH_RECHARGE_MULTIPLIER(PLAYER_ID(), 1.f);
	SET_ENTITY_HEALTH(PLAYER_ID(), GET_PED_MAX_HEALTH(PLAYER_ID()), 0);
}

static void OnStart()
{
	SET_PLAYER_HEALTH_RECHARGE_MULTIPLIER(PLAYER_ID(), .0f);
	SET_ENTITY_HEALTH(GET_PLAYER_PED(PLAYER_ID()), 101, 0);
	SET_PED_ARMOUR(GET_PLAYER_PED(PLAYER_ID()), 0);

}

static void OnTick()
{
	static const Hash model = GET_HASH_KEY("prop_ecola_can");
	REQUEST_MODEL(model);

	Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
	CREATE_AMBIENT_PICKUP(GET_HASH_KEY("PICKUP_HEALTH_STANDARD"), playerPos.x + g_random.GetRandomInt(-30, 30),
		playerPos.y + g_random.GetRandomInt(-30, 30), playerPos.z + g_random.GetRandomInt(5, 10), 0, GET_ENTITY_MAX_HEALTH(GET_PLAYER_PED(PLAYER_ID())), model, false, true);

	if (g_random.GetRandomInt(0, 200) == 69) {
		for (Ped ped : GetAllPeds())
		{
			if (!IS_PED_DEAD_OR_DYING(ped, true) && GET_ENTITY_HEALTH(ped) > 101)
			{
				SET_ENTITY_HEALTH(ped, 101, 0);
				SET_PED_ARMOUR(ped, 0);
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_AUSTRALIENGUY, OnStart, OnStop, OnTick);