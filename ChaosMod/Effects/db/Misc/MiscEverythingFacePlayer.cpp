#include <stdafx.h>

static void OnTick()
{
	auto playerPos = GET_ENTITY_COORDS(PLAYER_PED_ID(), false);
	int playerPosLength = playerPos.x + playerPos.y + playerPos.z;

	Entity entities[1024];
	int size = 0;

	for (auto ped : GetAllPeds())
	{
		if (!IS_PED_A_PLAYER(ped))
		{
			entities[size++] = ped;
		}
	}

	for (auto veh : GetAllVehs())
	{
		entities[size++] = veh;
	}

	for (auto object : GetAllProps())
	{
		entities[size++] = object;
	}

	for (int i = 0; i < size; i++)
	{
		auto entity = entities[i];
		auto entityPos = GET_ENTITY_COORDS(entity, false);

		float x = GET_ENTITY_PITCH(entity), y = GET_ENTITY_HEADING(entity), z = GET_ENTITY_ROLL(entity);

		float angle = std::acos(std::inner_product(&entityPos.x, &entityPos.z, &playerPos.x, .0f)
			/ ((entityPos.x + entityPos.y + entityPos.z) * playerPosLength));

		float heading = std::atan2(y * std::sin(angle) - x * z * (1 - std::cos(angle)),
			1 - (std::pow(y, 2) + std::pow(z, 2)) * (1 - std::cos(angle)));

		if (!std::isnan(heading))
		{
			SET_ENTITY_HEADING(entity, heading);
		}
	}
}