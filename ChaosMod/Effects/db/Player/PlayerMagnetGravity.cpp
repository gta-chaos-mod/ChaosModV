/*
	Effect By OnlyRealNubs
*/

#include <stdafx.h>

static enum MagnetGravityState
{
	PULL = -1,
	IDLE,
	PUSH
} g_EffectState;

static DWORD64 lastChangeTick;
static const int changeInterval = 2000;

static const float maxDistance  = 60.f;
static const float minDistance  = 40.f;

static void OnStart()
{
	g_EffectState = MagnetGravityState::IDLE;
	lastChangeTick = GetTickCount64();
}

static void OnStop()
{
	SET_PLAYER_INVINCIBLE(PLAYER_ID(), false);
	for (Entity ent : GetAllEntitiesArray())
	{
		SET_ENTITY_INVINCIBLE(ent, false);
	}
}

static void OnTick()
{
	DWORD64 currentTick = GetTickCount64();
	if (lastChangeTick < currentTick - changeInterval)
	{
		lastChangeTick = currentTick;
		g_EffectState = (MagnetGravityState)(g_EffectState >= 1 ? -1 : g_EffectState + 1);
	}

	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerCoord = GET_ENTITY_COORDS(playerPed, false);
	
	int count = 10;
	std::vector<Entity> entities;
	
	for (Entity ent : GetAllEntitiesArray())
	{
		if (IS_ENTITY_A_PED(ent))
		{
			if (ent != playerPed)
			{
				entities.push_back(ent);
			}
		}
		else if (IS_ENTITY_A_VEHICLE(ent))
		{
			if (!IS_PED_IN_VEHICLE(playerPed, ent, false))
			{
				entities.push_back(ent);
			}
		}
		else
		{
			entities.push_back(ent);
		}
	}

	if (g_EffectState != IDLE)
	{
		SET_PLAYER_INVINCIBLE(PLAYER_ID(), true);

		float randomDistance = g_Random.GetRandomFloat(minDistance, maxDistance);
		for (Entity entity : entities)
		{
			Vector3 entityCoord = GET_ENTITY_COORDS(entity, false);

			float distance      = GET_DISTANCE_BETWEEN_COORDS(playerCoord.x, playerCoord.y, playerCoord.z, entityCoord.x,
															  entityCoord.y, entityCoord.z, true);
			if (distance < maxDistance)
			{
				if (IS_ENTITY_A_PED(entity) && !IS_PED_RAGDOLL(entity))
				{
					SET_PED_TO_RAGDOLL(entity, 5000, 5000, 0, true, true, false);
				}
				Memory::ApplyForceToEntity(entity, 3, (entityCoord.x - playerCoord.x) * (float)g_EffectState,
				                           (entityCoord.y - playerCoord.y) * (float)g_EffectState,
				                           (entityCoord.z - playerCoord.z) * (float)g_EffectState,
										   0, 0, 0, false, false, true, true, false, true);

				if (IS_ENTITY_A_MISSION_ENTITY(entity))
				{
					SET_ENTITY_INVINCIBLE(entity, true);
				}

				if (--count <= 0)
				{
					WAIT(0);

					count = 10;
				}
			}
		}
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Pulsating Gravity Field",
		.Id = "player_magnetgravity",
		.IsTimed = true,
		.IsShortDuration = true,
		.EffectCategory = EEffectCategory::Gravity
	}
);