/*
    Effect by MoneyWasted
    Special Thanks to Parik for the Help :)
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

#include "Memory/Water.h"

CHAOS_VAR CWaterQuad *WaterQuads;
CHAOS_VAR std::vector<float> WaterHeights;

CHAOS_VAR CRiverQuad *RiverQuads;
CHAOS_VAR std::vector<float> RiverHeights;

static void OnStart()
{
	WaterHeights.reserve(MAXWATERQUADS);

	WaterQuads = Memory::GetAllWaterQuads();
	if (WaterQuads)
	{
		for (int i = 0; i < MAXWATERQUADS; i++)
		{
			WaterHeights.push_back(WaterQuads[i].Z); // Save Water Height
			WaterQuads[i].Z = -1000.0f;              // Remove Water
		}
	}

	RiverQuads = Memory::GetAllRiverQuads();
	if (RiverQuads)
	{
		char buffer[256];
		for (int i = 0; i < MAXRIVERQUADS; i++)
		{
			sprintf_s(buffer, "RiverQuads[%d]: 0x%p", i, &RiverQuads[i]);
			LOG(buffer);
		}
	}
}

static void OnStop()
{
	if (WaterQuads && !WaterHeights.empty())
	{
		for (int i = 0; i < MAXWATERQUADS; i++)
		{
			WaterQuads[i].Z = WaterHeights[i]; // Restore Water
			WaterHeights[i] = 0.0f;            // Clear Saved Height
		}

		WaterHeights.clear();
		WaterHeights.shrink_to_fit();
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