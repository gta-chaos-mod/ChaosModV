#pragma once

#include "Handle.h"
#include "Memory.h"

struct MinimapData
{
	char Name[100];
	float PosX;
	float PosY;
	float SizeX;
	float SizeY;
	char AlignX;
	char AlignY;
};

namespace Memory
{
	inline void SetRadarOffsetX(float xOffset)
	{
		static MinimapData *data                    = nullptr;
		static MinimapData defaultValues[3]         = {};

		static void(__cdecl * refreshMinimapFunc)() = nullptr;

		if (!data)
		{
			Handle handle = Memory::FindPattern(
				"?? 8D 15 ?? ?? ?? ?? ?? 6B C9 78 8B 44 ?? ?? 89 03 8B 44 ?? ?? 89 43 04 8A 4C ?? ??");
			handle = handle.At(2).Into();
			if (!handle.IsValid())
			{
				return;
			}

			data             = handle.Get<MinimapData>();

			defaultValues[0] = data[0]; // minimap
			defaultValues[1] = data[1]; // minimap_mask
			defaultValues[2] = data[2]; // minimap_blur

			handle = Memory::FindPattern("?? 89 5C ?? ?? 57 ?? 83 EC ?? ?? 8D 3D ?? ?? ?? ?? ?? 8D ?? ?? ?? E8");
			if (!handle.IsValid())
			{
				return;
			}

			refreshMinimapFunc = handle.Get<void()>();
		}

		data[0].PosX = defaultValues[0].PosX + xOffset; // minimap
		data[1].PosX = defaultValues[1].PosX + xOffset; // minimap_mask
		data[2].PosX = defaultValues[2].PosX + xOffset; // minimap_blur

		refreshMinimapFunc();
	}
}