/*
	Effect by Gorakh
*/

#include <stdafx.h>

static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();
	if (DOES_ENTITY_EXIST(playerPed) && !IS_ENTITY_DEAD(playerPed, 0) && IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(playerPed, false);
		if (GET_IS_VEHICLE_ENGINE_RUNNING(playerVeh))
		{
			for (Hash hash : { GET_HASH_KEY("SP0_TOTAL_CASH"), GET_HASH_KEY("SP1_TOTAL_CASH"), GET_HASH_KEY("SP2_TOTAL_CASH")})
			{
				int money;
				if (STAT_GET_INT(hash, &money, -1))
				{
					int moneyToDrain = GET_ENTITY_SPEED(playerVeh) * 0.9f;
					if (moneyToDrain > 0)
					{
						money -= moneyToDrain;
						money = max(money, 0);
						STAT_SET_INT(hash, money, true);

						if (money == 0)
						{
							SET_VEHICLE_ENGINE_HEALTH(playerVeh, 0.f);
						}

						DISPLAY_CASH(true);
					}
				}
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_MONEY_IS_FUEL, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Money Fuel",
		.Id = "player_money_is_fuel",
		.IsTimed = true
	}
);