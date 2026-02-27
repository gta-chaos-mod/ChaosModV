/*
    Effect by MoneyWasted
    Special Thanks to Parik for the Help :)
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

#include "Memory/Water.h"

CHAOS_VAR CWaterQuad *WaterQuads = nullptr;
CHAOS_VAR std::vector<float> WaterQuadHeights;

CHAOS_VAR CRiverEntity *RiverEntities = nullptr;
CHAOS_VAR std::vector<float> RiverEntityHeights;

constexpr float WATER_REMOVAL_DEPTH = -1000.0f;

static void OnStart()
{
	// Pre-allocate vectors to avoid reallocations
	WaterQuadHeights.resize(MAXWATERQUADS);
	RiverEntityHeights.resize(MAXRIVERENTITIES);

	// Process water quads
	if (WaterQuads = Memory::GetAllWaterQuads())
	{
		for (int i = 0; i < MAXWATERQUADS; i++)
		{
			WaterQuadHeights[i] = WaterQuads[i].Z;
			WaterQuads[i].Z     = WATER_REMOVAL_DEPTH;
		}
	}

	// Process river entities
	if (RiverEntities = Memory::GetAllRiverEntities())
	{
		for (int i = 0; i < MAXRIVERENTITIES; i++)
		{
			if (RiverEntities[i].Entity)
			{
				RiverEntityHeights[i]               = RiverEntities[i].Entity->Position.Z;
				RiverEntities[i].Entity->Position.Z = WATER_REMOVAL_DEPTH;
			}
		}
	}
}

static void OnStop()
{
	// Restore water quads
	if (WaterQuads)
		for (int i = 0; i < MAXWATERQUADS; i++)
			WaterQuads[i].Z = WaterQuadHeights[i];

	// Restore river entities
	if (RiverEntities)
		for (int i = 0; i < MAXRIVERENTITIES; i++)
			if (RiverEntities[i].Entity)
				RiverEntities[i].Entity->Position.Z = RiverEntityHeights[i];

	// Clear vectors
	WaterQuadHeights.clear();
	RiverEntityHeights.clear();
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, nullptr, 
    {
        .Name = "Drought",
        .Id = "misc_remove_water",
        .IsTimed = true
    }
);