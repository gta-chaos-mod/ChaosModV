/*
    Effect by MoneyWasted
    Special Thanks to Parik for the Help :)
*/

#include <stdafx.h>

#include "Effects/Register/RegisterEffect.h"

struct CWaterQuad
{
	short MinX;   // 0x0
	short MinY;   // 0x2
	short MaxX;   // 0x4
	short MaxY;   // 0x6
	uint Color;   // 0x8
	char unk1[4]; // 0xC
	char unk2[4]; // 0x10
	float Z;      // 0x14
	uint Flags;   // 0x18
};
static_assert(sizeof(CWaterQuad) == 0x1C);

CHAOS_VAR CWaterQuad *WaterQuads;
CHAOS_VAR std::vector<float> WaterHeights;

static CWaterQuad *GetWaterQuads()
{
	static Handle handle = Memory::FindPattern("? 6B C9 1C ? 03 0D ? ? ? ? 66 ? 03 C5 66 89 05 ? ? ? ?", "48 8B 15 ?? ?? ?? ?? 0F 57 C0 F3 44 0F 10 05");
	if (handle.IsValid())
		return *handle.At(IsLegacy() ? 6 : 2).Into().Get<CWaterQuad *>();
	return nullptr;
}

static void OnStart()
{
	WaterQuads = GetWaterQuads();
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