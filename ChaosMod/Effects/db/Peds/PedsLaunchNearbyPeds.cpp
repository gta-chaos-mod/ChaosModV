#include <stdafx.h>

static struct SpacePed
{
	SpacePed(Ped ped, Vector3 vel) : Ped(ped), Vel(vel)
	{

	}

	Ped Ped;
	Vector3 Vel;
};

static void OnStart()
{
	std::vector<SpacePed> spacePeds;

	// Ragdoll peds first, wait a bit so game registers them as ragdolled properly, then launch up
	// Also apply their previous velocity afterwards

	for (Ped ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			Vector3 vel = GET_ENTITY_VELOCITY(ped);

			CLEAR_PED_TASKS_IMMEDIATELY(ped);

			SET_PED_TO_RAGDOLL(ped, 10000, 10000, 0, true, true, false);

			spacePeds.emplace_back(ped, vel);
		}
	}

	WAIT(0);

	for (const SpacePed& spacePed : spacePeds)
	{
		const Vector3& vel = spacePed.Vel;

		SET_ENTITY_VELOCITY(spacePed.Ped, vel.x, vel.y, 100.f);
	}
}

static RegisterEffect registerEffect(EFFECT_LAUNCH_ALL_PEDS, OnStart, EffectInfo
	{
		.Name = "Launch All Nearby Peds Up",
		.Id = "peds_launchnearby"
	}
);