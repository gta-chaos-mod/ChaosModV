/*
	Effect by Moxi, based on "Detonate Current Vehicle", modified
*/

#include <stdafx.h>

#define LAUNCH_TIMER 5000

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();
	static const Hash parachuteHash = GET_HASH_KEY("GADGET_PARACHUTE");

	CLEAR_PED_TASKS_IMMEDIATELY(playerPed);
	SET_PED_TO_RAGDOLL(playerPed, 10000, 10000, 0, true, true, false);
	GIVE_WEAPON_TO_PED(playerPed, parachuteHash, 1, true, false);

	//REQUEST_NAMED_PTFX_ASSET("core"); Doesn't seem to be necessary
	
	int lastTimestamp = GET_GAME_TIMER();
	int launchTimer = LAUNCH_TIMER;
	int beepTimer = LAUNCH_TIMER;
	while (true)
	{
		SET_ENTITY_INVINCIBLE(playerPed, true);

		int curTimestamp = GET_GAME_TIMER();

		if ((launchTimer -= curTimestamp - lastTimestamp) < beepTimer)
		{
			beepTimer *= .8f;

			USE_PARTICLE_FX_ASSET("core");
			PLAY_SOUND_FROM_ENTITY(-1, "Beep_Red", playerPed, "DLC_HEIST_HACKING_SNAKE_SOUNDS", true, false);
			START_PARTICLE_FX_LOOPED_ON_ENTITY("exp_air_molotov", playerPed, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, .7f, false, false, false);
			SET_ENTITY_VELOCITY(playerPed, 0.f, 0.f, 5.f);
		}

		if (launchTimer <= 0)
		{
			USE_PARTICLE_FX_ASSET("core");
			START_PARTICLE_FX_LOOPED_ON_ENTITY("exp_air_rpg", playerPed, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 2.f, false, false, false);
			START_PARTICLE_FX_LOOPED_ON_ENTITY("exp_air_molotov", playerPed, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 5.f, false, false, false);
			SET_ENTITY_VELOCITY(playerPed, 0.f, 0.f, 100.f);	// Increasing the z value doesn't seem to affect the force anymore while ragdolling. 

			break;
		}

		lastTimestamp = curTimestamp;

		WAIT(0);
	}

	SET_ENTITY_INVINCIBLE(playerPed, false);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_ROCKET, OnStart, EffectInfo
	{
		.Name = "Rocket Man",
		.Id = "player_rocket"
	}
);