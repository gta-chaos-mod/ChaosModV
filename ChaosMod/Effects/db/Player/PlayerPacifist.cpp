#include <stdafx.h>
//Effect by ProfessorBiddle, requested thousands of times on discord
static void OnTick()
{
	Player player = PLAYER_ID();

	Entity target;

	STOP_GAMEPLAY_CAM_SHAKING(true);

	if (GET_ENTITY_PLAYER_IS_FREE_AIMING_AT(player, &target))
	{
		if ((IS_ENTITY_A_PED(target) || IS_ENTITY_A_VEHICLE(target)) && !IS_ENTITY_DEAD(target, false))
		{
			Ped playerPed = PLAYER_PED_ID();
			SET_ENTITY_HEALTH(playerPed, 0, 0);
		}
	}
}
static RegisterEffect registerEffect(EFFECT_PLAYER_PACIFIST, nullptr, nullptr, OnTick);