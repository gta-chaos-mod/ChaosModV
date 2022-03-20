/*
	Effect by MoneyWasted
	Special Thanks to Parik for the Help :)
*/

#include <stdafx.h>

uint64_t WaterPattern = Memory::FindPattern("? 6B C9 1C ? 03 0D ? ? ? ? 66 ? 03 C5 66 89 05 ? ? ? ?").Addr();
uint64_t WaterPointer;

std::vector<float> WaterHeights;

static void OnStart()
{
	WaterPointer = *(DWORD64*)((WaterPattern + *(DWORD*)(WaterPattern + 0x7)) + 0xB);
	for (int i = 0; i < 821; i++) // 821 = Max Water Items
	{
		uint64_t Water = WaterPointer + (i * 0x1C);
		WaterHeights.push_back(*(float*)(Water + 0x14));
		*(float*)(Water + 0x14) = -1000.0f; // Remove Water
	}
}

static void OnStop()
{
	for (int i = 0; i < 821; i++) // 821 = Max Water Items
	{
		uint64_t Water = WaterPointer + (i * 0x1C);
		*(float*)(Water + 0x14) = WaterHeights.at(i); // Restore Water
	}
	WaterHeights.clear();
}

static RegisterEffect registerEffect(EFFECT_MISC_WATER, OnStart, OnStop, nullptr, EffectInfo
	{
		.Name = "Drought",
		.Id = "remove_water",
		.IsTimed = true
	}
);