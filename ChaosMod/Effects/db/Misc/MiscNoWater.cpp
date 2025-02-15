/*
    Effect by MoneyWasted
    Special Thanks to Parik for the Help :)
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

#include "Memory/Water.h"

CHAOS_VAR CWaterQuad *WaterQuads;
CHAOS_VAR std::vector<float> WaterHeights;

static void OnStart()
{
	WaterQuads = Memory::GetAllWaterQuads();
	if (WaterQuads)
	{
		for (int i = 0; i < 821; i++) // 821 = Max Water Items
		{
			WaterHeights.push_back(WaterQuads[i].Z); // Save Water Heights
			WaterQuads[i].Z = -1000.0f;              // Remove Water
		}
	}
}

static void OnStop()
{
	if (WaterQuads)
	{
		for (int i = 0; i < 821; i++)             // 821 = Max Water Items
			WaterQuads[i].Z = WaterHeights.at(i); // Restore Water
		WaterHeights.clear();                     // Clear Storage Vector
	}
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, nullptr, 
	{
		.Name = "Drought",
		.Id = "misc_remove_water",
		.IsTimed = true
	}
);