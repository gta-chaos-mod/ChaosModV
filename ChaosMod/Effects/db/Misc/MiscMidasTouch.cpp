/*
	Effect by Lucas7yoshi, modified
*/

#include <stdafx.h>

static void OnTick()
{
	Ped playerPed = PLAYER_PED_ID();
	int cE = playerPed; // COMPARE ENTITY

	if (IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		cE = GET_VEHICLE_PED_IS_IN(playerPed, false);
		SET_VEHICLE_COLOURS(cE, 158, 158); // 158 = Pure Gold
		SET_VEHICLE_EXTRA_COLOURS(cE, 160, 158);
	}

	for (Vehicle veh : GetAllVehs())
	{
		if (IS_ENTITY_TOUCHING_ENTITY(cE, veh))
		{
			SET_VEHICLE_COLOURS(veh, 158, 158); // 158 = Pure Gold
			SET_VEHICLE_EXTRA_COLOURS(veh, 160, 158);
		}
	}

	static const auto model = GET_HASH_KEY("prop_money_bag_01");
	REQUEST_MODEL(model);
	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			if (!IS_ENTITY_A_MISSION_ENTITY(ped) || IS_CUTSCENE_PLAYING()) // lets some fun happen in cutscenes, in theorey.
			{
				if (IS_ENTITY_TOUCHING_ENTITY(cE, ped))
				{
					auto pos = GET_ENTITY_COORDS(ped, false);
					CREATE_AMBIENT_PICKUP(GET_HASH_KEY("PICKUP_MONEY_SECURITY_CASE"), pos.x, pos.y, pos.z, 0, 1000, model, false, true);
					SET_ENTITY_COORDS(ped, 0, 0, 0, 1, 0, 0, 1);
					SET_PED_AS_NO_LONGER_NEEDED(&ped);
					DELETE_PED(&ped);
				}
			}
		}
	}

	for (int prop : GetAllProps())
	{
		if (IS_ENTITY_TOUCHING_ENTITY(cE, prop) && !IS_PED_CLIMBING(cE))
		{
			if (!IS_ENTITY_A_MISSION_ENTITY(prop) || IS_CUTSCENE_PLAYING())
			{
				if (GET_ENTITY_ATTACHED_TO(prop) == false)
				{
					auto pos = GET_ENTITY_COORDS(prop, false);
					CREATE_AMBIENT_PICKUP(GET_HASH_KEY("PICKUP_MONEY_SECURITY_CASE"), pos.x, pos.y, pos.z, 0, 1000, model, false, true);
					SET_ENTITY_COORDS(prop, 0, 0, 0, 1, 0, 0, 1);
					SET_ENTITY_AS_NO_LONGER_NEEDED(&prop);
					DELETE_ENTITY(&prop);
				}
			}
		}
	}

	if (IS_PED_ARMED(playerPed, 7))
	{
		Hash weaponHash;
		GET_CURRENT_PED_WEAPON(playerPed, &weaponHash, false);
		SET_PED_WEAPON_TINT_INDEX(playerPed, weaponHash, 2);
	}

}

static RegisterEffect registerEffect(EFFECT_MIDAS_TOUCH, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Midas Touch",
		.Id = "misc_midas",
		.IsTimed = true
	}
);