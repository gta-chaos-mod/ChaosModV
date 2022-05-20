/*
    Effect by Reguas
*/

#include <stdafx.h>

#include "Util/Player.h"

struct Location
{
	Vector3 coordinates;
	float rotation;
	float speed;
};

static std::vector<Location> allPossibleJumps = {

	{ { 46.101, 6530.031, 30.713 }, 137.332, 40.f },    // 1
	{ { -186.248, 6554.466, 10.446 }, 314.389, 30.f },  // 2
	{ { 490.106, 4309.650, 54.884 }, 54.039, 45.f },    // 3
	{ { -953.295, 4166.992, 136.374 }, 51.525, 30.f },  // 4
	{ { 3335.163, 5148.697, 17.637 }, 276.721, 45.f },  // 5
	{ { 1684.881, 3141.746, 42.871 }, 24.974, 45.f },   // 6
	{ { 1680.338, 2316.249, 74.705 }, 357.364, 43.f },  // 7
	{ { 1780.440, 2056.524, 65.945 }, 195.309, 60.f },  // 8
	{ { 2001.575, 1920.334, 91.608 }, 57.712, 45.f },   // 9
	{ { -2.917, 1699.827, 226.620 }, 325.440, 45.f },   // 10
	{ { -1447.177, 412.317, 109.067 }, 191.007, 55.f }, // 11
	{ { -1081.151, 11.979, 50.056 }, 257.962, 22.f },   // 12
	{ { -713.306, -49.739, 37.063 }, 110.136, 42.f },   // 13
	{ { -588.852, -92.791, 41.684 }, 153.347, 30.f },   // 14
	{ { -1589.183, -748.284, 20.791 }, 79.817, 48.f },  // 15
	{ { 46.478, -780.456, 43.524 }, 249.806, 30.f },    // 16
	{ { 303.248, -618.176, 42.797 }, 249.437, 40.f },   // 17
	{ { 563.992, -583.787, 43.544 }, 187.271, 45.f },   // 18
	{ { -285.071, -763.947, 52.595 }, 241.151, 45.f },  // 19
	{ { -873.079, -848.673, 18.503 }, 106.121, 40.f },  // 20
	{ { -617.539, -1074.971, 21.727 }, 74.128, 40.f },  // 21
	{ { 2.338, -1038.557, 37.502 }, 70.644, 35.f },     // 22
	{ { -440.262, -1178.323, 52.588 }, 175.859, 50.f }, // 23
	{ { -533.043, -1480.427, 11.122 }, 248.903, 45.f }, // 24
	{ { -453.039, -1380.261, 29.779 }, 180.118, 45.f }, // 25
	{ { -570.175, -1533.781, 0.603 }, 72.475, 50.f },   // 26
	{ { -423.742, -1564.256, 24.752 }, 350.530, 45.f }, // 27
	{ { -980.685, -2491.565, 13.898 }, 151.314, 60.f }, // 28
	{ { -860.708, -2566.927, 13.785 }, 331.449, 60.f }, // 29
	{ { -958.207, -2766.583, 13.693 }, 151.829, 45.f }  // 30

};

static void OnStart()
{
	Ped playerPed = PLAYER_PED_ID();
	Vehicle veh;

	Location loc = allPossibleJumps.at(g_Random.GetRandomInt(0, allPossibleJumps.size() - 1));

	TeleportPlayer(loc.coordinates);

	if (!IS_PED_IN_ANY_VEHICLE(playerPed, false))
	{
		Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

		veh               = CreatePoolVehicle(GET_HASH_KEY("BATI"), playerPos.x, playerPos.y, playerPos.z,
		                                      GET_ENTITY_HEADING(PLAYER_PED_ID()));

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
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
    {
        .Name = "Make Random Stunt Jump",
        .Id = "player_tp_stunt",
        .EffectGroupType = EEffectGroupType::Teleport
    }
);