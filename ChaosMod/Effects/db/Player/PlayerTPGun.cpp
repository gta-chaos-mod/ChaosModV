#include <stdafx.h>

/**
 * Effect by: https://github.com/Elias-Graf
 **/

static void OnTick()
{
	static auto player = PLAYER_PED_ID();
	static auto playerShoot = false;

	if (IS_PED_SHOOTING(player))
	{
		playerShoot = true;
	}

	// Bullet Impact Coords
	auto BIC = Vector3();
	// If the player shot, and the bullet impacted (takes some time), teleport the
	// player
	if (playerShoot && GET_PED_LAST_WEAPON_IMPACT_COORD(player, &BIC))
	{
		playerShoot = false;

		SET_ENTITY_COORDS(player, BIC.x, BIC.y, BIC.z, 1, 0, 0, true);
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_PORTAL_GUN, nullptr, nullptr, OnTick);