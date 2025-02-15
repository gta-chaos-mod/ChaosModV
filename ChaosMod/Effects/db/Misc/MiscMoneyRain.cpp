#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

static void OnStop()
{
	SET_MODEL_AS_NO_LONGER_NEEDED("prop_money_bag_01"_hash);
}

static void OnTick()
{
	static const Hash model = "prop_money_bag_01"_hash;
	REQUEST_MODEL(model);

	Vector3 playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
	CREATE_AMBIENT_PICKUP("PICKUP_MONEY_SECURITY_CASE"_hash, playerPos.x + g_Random.GetRandomInt(-20, 20),
	                      playerPos.y + g_Random.GetRandomInt(-20, 20), playerPos.z + g_Random.GetRandomInt(5, 10), 0,
	                      1000, model, false, true);
}

// clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, 
	{
		.Name = "Money Rain",
		.Id = "player_moneydrops",
		.IsTimed = true
	}
);