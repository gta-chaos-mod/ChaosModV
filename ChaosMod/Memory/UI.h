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
	static void(__cdecl *GetRefreshMinimapFunc())()
	{
		Handle handle = Memory::FindPattern("?? 89 5C ?? ?? 57 ?? 83 EC ?? ?? 8D 3D ?? ?? ?? ?? ?? 8D ?? ?? ?? E8",
		                                    "56 57 48 83 EC 48 E8 ?? ?? ?? ?? 89");
		if (!handle.IsValid())
			return nullptr;

		return handle.Get<void()>();
	}

	inline void SetRadarParams(std::function<void(MinimapData &, MinimapData)> process)
	{
		static MinimapData defaultMinimap[3];
		static void(__cdecl * refreshMinimapFunc)();
		static auto minimapData = []() -> MinimapData *
		{
			Handle handle = Memory::FindPattern(
			    "?? 8D 15 ?? ?? ?? ?? ?? 6B C9 78 8B 44 ?? ?? 89 03 8B 44 ?? ?? 89 43 04 8A 4C ?? ??",
			    "48 8D 3D ?? ?? ?? ?? F2 0F 10 44 3B 6C");
			handle = handle.At(2).Into();
			if (!handle.IsValid())
				return nullptr;

			auto minimapData = handle.Get<MinimapData>();
			for (size_t i = 0; i < 3; i++)
				defaultMinimap[i] = minimapData[i];

			refreshMinimapFunc = GetRefreshMinimapFunc();

			return minimapData;
		}();

		if (!minimapData || !refreshMinimapFunc)
			return;

		for (size_t i = 0; i < 3; i++)
			process(minimapData[i], defaultMinimap[i]);

		refreshMinimapFunc();
	}

	inline void ResetRadar()
	{
		SetRadarParams([](MinimapData &dst, MinimapData src) { dst = src; });
	}

	inline void SetRadarOffset(float xOffset, float yOffset)
	{
		SetRadarParams(
		    [&](MinimapData &dst, MinimapData src)
		    {
			    dst.PosX = src.PosX + xOffset;
			    dst.PosY = src.PosY + yOffset;
		    });
	}

	inline void MultiplyRadarSize(float multiplier, float offsetX = 0.f, float offsetY = 0.f)
	{
		SetRadarParams(
		    [&](MinimapData &dst, MinimapData src)
		    {
			    dst.SizeX = src.SizeX * multiplier;
			    dst.SizeY = src.SizeY * multiplier;
			    dst.PosX  = src.PosX * multiplier + offsetX;
			    dst.PosY  = src.PosY * multiplier + offsetY;
		    });
	}
}