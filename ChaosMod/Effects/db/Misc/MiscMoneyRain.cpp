#include <stdafx.h>

static void OnStop()
{
	SET_MODEL_AS_NO_LONGER_NEEDED(GET_HASH_KEY("prop_money_bag_01"));
}

static void OnTick()
{
	static const auto model = GET_HASH_KEY("prop_money_bag_01");
	REQUEST_MODEL(model);

	auto playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
	CREATE_AMBIENT_PICKUP(GET_HASH_KEY("PICKUP_MONEY_SECURITY_CASE"), playerPos.x + Random::GetRandomInt(-20, 20),
		playerPos.y + Random::GetRandomInt(-20, 20), playerPos.z + Random::GetRandomInt(5, 10), 0, 1000, model, false, true);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_MONEYDROPS, nullptr, OnStop, OnTick);