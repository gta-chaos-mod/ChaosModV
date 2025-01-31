/*
    Effect by Reguas
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"
#include "Util/Player.h"
#include "Util/Vehicle.h"

#include "PlayerRandomStuntJump.h"

struct Location
{
	Vector3 coordinates;
	float rotation;
	float speed;
};

CHAOS_VAR std::vector<Location> allPossibleJumps = {
	{ { 46.101f, 6530.031f, 30.713f }, 137.332f, 40.f },    // 1
	{ { -186.248f, 6554.466f, 10.446f }, 314.389f, 30.f },  // 2
	{ { 490.106f, 4309.650f, 54.884f }, 54.039f, 45.f },    // 3
	{ { -953.295f, 4166.992f, 136.374f }, 51.525f, 30.f },  // 4
	{ { 3335.163f, 5148.697f, 17.637f }, 276.721f, 45.f },  // 5
	{ { 1684.881f, 3141.746f, 42.871f }, 24.974f, 45.f },   // 6
	{ { 1680.338f, 2316.249f, 74.705f }, 357.364f, 43.f },  // 7
	{ { 1780.440f, 2056.524f, 65.945f }, 195.309f, 60.f },  // 8
	{ { 2001.575f, 1920.334f, 91.608f }, 57.712f, 45.f },   // 9
	{ { -2.917f, 1699.827f, 226.620f }, 325.440f, 45.f },   // 10
	{ { -1447.177f, 412.317f, 109.067f }, 191.007f, 55.f }, // 11
	{ { -1081.151f, 11.979f, 50.056f }, 257.962f, 22.f },   // 12
	{ { -713.306f, -49.739f, 37.063f }, 110.136f, 42.f },   // 13
	{ { -588.852f, -92.791f, 41.684f }, 153.347f, 30.f },   // 14
	{ { -1589.183f, -748.284f, 20.791f }, 79.817f, 48.f },  // 15
	{ { 46.478f, -780.456f, 43.524f }, 249.806f, 30.f },    // 16
	{ { 303.248f, -618.176f, 42.797f }, 249.437f, 40.f },   // 17
	{ { 563.992f, -583.787f, 43.544f }, 187.271f, 45.f },   // 18
	{ { -285.071f, -763.947f, 52.595f }, 241.151f, 45.f },  // 19
	{ { -873.079f, -848.673f, 18.503f }, 106.121f, 40.f },  // 20
	{ { -617.539f, -1074.971f, 21.727f }, 74.128f, 40.f },  // 21
	{ { 2.338f, -1038.557f, 37.502f }, 70.644f, 35.f },     // 22
	{ { -440.262f, -1178.323f, 52.588f }, 175.859f, 50.f }, // 23
	{ { -533.043f, -1480.427f, 11.122f }, 248.903f, 45.f }, // 24
	{ { -453.039f, -1380.261f, 29.779f }, 180.118f, 45.f }, // 25
	{ { -570.175f, -1533.781f, 0.603f }, 72.475f, 50.f },   // 26
	{ { -423.742f, -1564.256f, 24.752f }, 350.530f, 45.f }, // 27
	{ { -980.685f, -2491.565f, 13.898f }, 151.314f, 60.f }, // 28
	{ { -860.708f, -2566.927f, 13.785f }, 331.449f, 60.f }, // 29
	{ { -958.207f, -2766.583f, 13.693f }, 151.829f, 45.f }  // 30
};

void OnStartMakeRandomStuntJump()
{
	Ped playerPed = PLAYER_PED_ID();
	Vehicle veh;

	Location loc = allPossibleJumps.at(g_Random.GetRandomInt(0, allPossibleJumps.size() - 1));

	TeleportPlayer(loc.coordinates);

	if (!IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

		veh =
		    CreateTempVehicle("bati"_hash, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(PLAYER_PED_ID()));

		SET_PED_INTO_VEHICLE(playerPed, veh, -1);
	}
	else
	{
		veh = GET_VEHICLE_PED_IS_IN(playerPed, false);
	}

	SET_ENTITY_VELOCITY(veh, 0, 0, 0);
	SET_ENTITY_ROTATION(veh, 0, 0, loc.rotation, 2, true);
	SET_VEHICLE_FORWARD_SPEED(veh, loc.speed);
}

// clang-format off
REGISTER_EFFECT(OnStartMakeRandomStuntJump, nullptr, nullptr, 
    {
        .Name = "Make Random Stunt Jump",
        .Id = "player_tp_stunt",
        .EffectGroupType = EffectGroupType::Teleport
    }
);