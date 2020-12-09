/*
    Effect by MoneyWasted
*/

#include <stdafx.h>

static void RandomStore(int random)
{
    switch (random)
    {
    case 0:
        TeleportPlayer(372.29217529297f, 326.39370727539f, 103.56636047363f);   // 247 Clinton
        break;
    case 1:
        TeleportPlayer(24.215274810791f, -1347.2624511719f, 29.497016906738f);  // 247 Strawberry
        break;
    case 2:
        TeleportPlayer(2557.1748046875f, 380.64489746094f, 108.62294006348f);   // 247 Palomino
        break;
    case 3:
        TeleportPlayer(-3038.2673339844f, 584.47491455078f, 7.908935546875f);   // 247 Ineseno
        break;
    case 4:
        TeleportPlayer(-3242.2670898438f, 999.76306152344f, 12.830704689026f);  // 247 Barbareno
        break;
    case 5:
        TeleportPlayer(549.44256591797f, 2671.2185058594f, 42.156513214111f);   // 247 Route 68
        break;
    case 6:
        TeleportPlayer(1959.9187011719f, 3740.0014648438f, 32.343738555908f);   // 247 Alhambra
        break;
    case 8:
        TeleportPlayer(1727.7840576172f, 6415.3408203125f, 35.037250518799f);   // 247 Senora
        break;
    case 9:
        TeleportPlayer(2677.9306640625f, 3279.3017578125f, 55.241123199463f);   // 247 Route 13
        break;
    case 10:
        TeleportPlayer(-2966.3012695313f, 391.58193969727f, 15.043300628662f);  // RobsLiquor Great Ocean
        break;
    case 11:
        TeleportPlayer(-1487.2850341797f, -376.92288208008f, 40.163436889648f); // RobsLiquor Prosperity
        break;
    case 12:
        TeleportPlayer(-1221.3229980469f, -908.12780761719f, 12.326356887817f); // RobsLiquor San Andreas
        break;
    case 13:
        TeleportPlayer(1134.0545654297f, -983.3251953125f, 46.415802001953f);   // RobsLiquor El Rancho
        break;
    case 14:
        TeleportPlayer(1165.2305908203f, 2710.9692382813f, 38.157665252686f);   // RobsLiquor Route 68
        break;
    case 15:
        TeleportPlayer(-705.91625976563f, -913.41326904297f, 19.215585708618f); // LTD Vespucci
        break;
    case 16:
        TeleportPlayer(-46.958980560303f, -1758.9643554688f, 29.420999526978f); // LTD Davis
        break;
    case 17:
        TeleportPlayer(1165.1630859375f, -323.87414550781f, 69.205047607422f);  // LTD Mirror Park
        break;
    case 18:
        TeleportPlayer(-1819.5125732422f, 793.64141845703f, 138.08486938477f);  // LTD Banham Canyon
        break;
    case 19:
        TeleportPlayer(1697.1395263672f, 4923.4130859375f, 42.063632965088f);   // LTD Grapeseed
        break;
    default:
        TeleportPlayer(0.0f, 0.0f, 0.0f); // Something broke :(
        break;
    }
}

static void OnStart()
{
    RandomStore(g_random.GetRandomInt(0, 19));
}

static RegisterEffect registerEffect(EFFECT_TP_TO_STORE, OnStart, nullptr, nullptr);