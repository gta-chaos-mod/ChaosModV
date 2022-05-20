/*
    Effect by MoneyWasted
*/

#include <stdafx.h>

#include "Util/Player.h"

static std::vector<Vector3> allPossibleStores = {

	// Convenience Stores
	{ 372.29217529297f, 326.39370727539f, 103.56636047363f },   // 247 Clinton
	{ 24.215274810791f, -1347.2624511719f, 29.497016906738f },  // 247 Strawberry
	{ 2557.1748046875f, 380.64489746094f, 108.62294006348f },   // 247 Palomino
	{ -3038.2673339844f, 584.47491455078f, 7.908935546875f },   // 247 Ineseno
	{ -3242.2670898438f, 999.76306152344f, 12.830704689026f },  // 247 Barbareno
	{ 549.44256591797f, 2671.2185058594f, 42.156513214111f },   // 247 Route 68
	{ 1959.9187011719f, 3740.0014648438f, 32.343738555908f },   // 247 Alhambra
	{ 1727.7840576172f, 6415.3408203125f, 35.037250518799f },   // 247 Senora
	{ 2677.9306640625f, 3279.3017578125f, 55.241123199463f },   // 247 Route 13
	{ -2966.3012695313f, 391.58193969727f, 15.043300628662f },  // RobsLiquor Great Ocean
	{ -1487.2850341797f, -376.92288208008f, 40.163436889648f }, // RobsLiquor Prosperity
	{ -1221.3229980469f, -908.12780761719f, 12.326356887817f }, // RobsLiquor San Andreas
	{ 1134.0545654297f, -983.3251953125f, 46.415802001953f },   // RobsLiquor El Rancho
	{ 1165.2305908203f, 2710.9692382813f, 38.157665252686f },   // RobsLiquor Route 68
	{ -705.91625976563f, -913.41326904297f, 19.215585708618f }, // LTD Vespucci
	{ -46.958980560303f, -1758.9643554688f, 29.420999526978f }, // LTD Davis
	{ 1165.1630859375f, -323.87414550781f, 69.205047607422f },  // LTD Mirror Park
	{ -1819.5125732422f, 793.64141845703f, 138.08486938477f },  // LTD Banham Canyon
	{ 1697.1395263672f, 4923.4130859375f, 42.063632965088f },   // LTD Grapeseed

	// Ammunitions
	{ 810.94f, -2157.19f, 29.62f },  // Ammunition (Cypress Flats)
	{ 21.15f, -1106.44f, 29.8f },    // Ammunition (PillBox Hill)
	{ 842.55f, -1033.72f, 28.19f },  // Ammunition (La Mesa)
	{ -662.42f, -935.19f, 21.83f },  // Ammunition (Cypress Flats)
	{ -1305.54f, -394.7f, 36.7f },   // Ammunition (MorningWood)
	{ 251.97f, -50.19f, 69.94f },    // Ammunition (Hawick)
	{ 2567.91f, 294.05f, 108.73f },  // Ammunition (Montañas Tataviam)
	{ 1693.32f, 3760.19f, 34.70f },  // Ammunition (Sandy Shores)
	{ -330.61f, 6084.02f, 31.45f },  // Ammunition (Sandy Shores)
	{ -1118.11f, 2698.71f, 18.55f }, // Ammunition (Sandy Shores)

	// Clothing Stores
	{ 72.3f, -1399.1f, 28.4f },
	{ -703.8f, -152.3f, 36.4f },
	{ -167.9f, -299.0f, 38.7f },
	{ 428.7f, -800.1f, 28.5f },
	{ -829.4f, -1073.7f, 10.3f },
	{ -1447.8f, -242.5f, 48.8f },
	{ 11.6f, 6514.2f, 30.9f },
	{ 123.6f, -219.4f, 53.6f },
	{ 1696.3f, 4829.3f, 41.1f },
	{ 618.1f, 2759.6f, 41.1f },
	{ 1190.6f, 2713.4f, 37.2f },
	{ -1193.4f, -772.3f, 16.3f },
	{ -3172.5f, 1048.1f, 19.9f },
	{ -1108.4f, 2708.9f, 18.1f }

};

static void OnStart()
{
	TeleportPlayer(allPossibleStores.at(g_Random.GetRandomInt(0, allPossibleStores.size() - 1)));
}

// clang-format off
REGISTER_EFFECT(OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Teleport to Random Store",
		.Id = "player_tp_store",
        .EffectGroupType = EEffectGroupType::Teleport
	}
);
