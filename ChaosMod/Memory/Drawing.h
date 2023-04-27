#pragma once

#include <stdafx.h>

#include "Util/Color.h"

// Thanks CitizenFX!
// Adapted by MoneyWasted for ChaosModV
// https://github.com/citizenfx/fivem/blob/master/code/components/extra-natives-five/src/Draw2dNatives.cpp

// This file is manually formatted.
// clang-format off

constexpr int MAX_LINES = 5000;

char *(*OG_AllocateDrawRect)(void *);
void (*OG_SetDrawRectCoords)(void *, float, float, float, float);

namespace Hooks
{
	inline void DrawLine(float x1, float y1, float x2, float y2, float width, int r, int g, int b, int a)
	{
		static char *ms_DrawRects             = 0;
		static int ms_DrawRectsSize           = 0;
		static int *ms_MainThreadFrameIndex   = 0;

		static void *ms_AllocateDrawRectAddr  = 0;
		static void *ms_SetDrawRectCoordsAddr = 0;

		if (!ms_DrawRects)
		{
			// Get our base handle at the beginning of the DRAW_RECT function.
			Handle handle = Memory::FindPattern("48 8B C4 48 89 58 08 57 48 83 EC 70 48 63");
			if (!handle.IsValid())
			{
				return;
			}

			ms_DrawRects             = handle.At(0x32).Into().Get<char>();
			ms_DrawRectsSize         = handle.At(0x2C).Value<int>();
			ms_MainThreadFrameIndex  = handle.At(0x0E).Into().Get<int>();

			ms_AllocateDrawRectAddr  = handle.At(0x56).Into().Get<void>();
			ms_SetDrawRectCoordsAddr = handle.At(0x92).Into().Get<void>();

			// At the instruction 'cmp edx, 500' in allocateDrawRect.
			handle = Memory::FindPattern("81 FA F4 01 00 00 73 13");
			if (!handle.IsValid())
			{
				return;
			}

			// Increase the number of lines that can be drawn.
			Memory::Write<int>(handle.At(2).Get<int>(), MAX_LINES);

			OG_AllocateDrawRect  = *(char *(*)(void *))ms_AllocateDrawRectAddr;
			OG_SetDrawRectCoords = *(void (*)(void *, float, float, float, float))ms_SetDrawRectCoordsAddr;
		}

		// Analysis in IDA shows that the return value of OG_AllocateDrawRect would be a structure.
		if (auto rect = OG_AllocateDrawRect(&ms_DrawRects[ms_DrawRectsSize * *ms_MainThreadFrameIndex]))
		{
			// Clamp each color value then combine them.
			uint32_t color = (
				(std::clamp(a, 0, 255) << 24) |
				(std::clamp(r, 0, 255) << 16) |
				(std::clamp(g, 0, 255) << 8)  |
				(std::clamp(b, 0, 255) << 0)
			);

			OG_SetDrawRectCoords(rect, x1, y1, x2, y2);

			// Set unknown structure pointers.
			*(uint32_t*)(rect + 0x34) &= 0xFA;
			*(uint32_t*)(rect + 0x34) |= 0x8A;
			*(float*)(rect + 0x1C)    = width;
			*(uint32_t*)(rect + 0x28) = color;
		}
	}

	inline void DrawLine(Vector3 firstPos, Vector3 secondPos, Color color, float thickness)
	{
		DrawLine(firstPos.x, firstPos.y, secondPos.x, secondPos.y, thickness, color.R, color.G, color.B, color.A);
	}
}