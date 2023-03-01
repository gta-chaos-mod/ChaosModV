#include <stdafx.h>

static Vector3 ms_BlackHolePos;
static float ms_fCurRadius;

static void OnStart()
{
	ms_BlackHolePos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
	ms_BlackHolePos.x += g_Random.GetRandomInt(-1000, 1000);
	ms_BlackHolePos.y += g_Random.GetRandomInt(-1000, 1000);
	ms_BlackHolePos.z += g_Random.GetRandomInt(400, 800);

	ms_fCurRadius = 0.f;
}

static void OnTick()
{
	if (ms_fCurRadius < 200.f)
	{
		ms_fCurRadius += 0.2f + GET_FRAME_TIME();
	}

	_DRAW_SPHERE(ms_BlackHolePos.x, ms_BlackHolePos.y, ms_BlackHolePos.z, ms_fCurRadius, 0, 0, 0, 1.f);
	SHAKE_GAMEPLAY_CAM("LARGE_EXPLOSION_SHAKE", 0.1f * ms_fCurRadius / 200.f);

	std::list<Entity> rgEntities;
	for (auto ped : GetAllPeds())
	{
		rgEntities.push_back(ped);
	}
	for (auto veh : GetAllVehs())
	{
		rgEntities.push_back(veh);
	}
	for (auto prop : GetAllProps())
	{
		rgEntities.push_back(prop);
	}

	auto playerVeh = GET_VEHICLE_PED_IS_IN(PLAYER_PED_ID(), false);
	for (auto entity : rgEntities)
	{
		auto pos = GET_ENTITY_COORDS(entity, false);

		if ((entity != playerVeh && !IS_ENTITY_A_MISSION_ENTITY(entity))
		    || GET_ENTITY_HEIGHT_ABOVE_GROUND(entity) > 2.f)
		{
			auto vel       = GET_ENTITY_VELOCITY(entity);
			Vector3 newVel = {
				(ms_BlackHolePos.x - pos.x) - (2.f * vel.x),
				(ms_BlackHolePos.y - pos.y) - (2.f * vel.y),
				(ms_BlackHolePos.z - pos.z) - (2.f * vel.z),
			};
			APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(entity, 0, newVel.x, newVel.y, newVel.z, true, false, true, true);
		}

		if (pos.DistanceTo(ms_BlackHolePos) < ms_fCurRadius)
		{
			if (IS_ENTITY_A_PED(entity))
			{
				SET_ENTITY_HEALTH(entity, 0, 0);
			}
			else if (IS_ENTITY_A_VEHICLE(entity))
			{
				EXPLODE_VEHICLE(entity, true, false);
			}

			if (!IS_ENTITY_A_MISSION_ENTITY(entity))
			{
				DELETE_ENTITY(&entity);
			}
		}
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Black Hole",
		.Id = "world_blackhole",
		.IsTimed = true,
		.IsShortDuration = true
    }
);