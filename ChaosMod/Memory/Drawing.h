#pragma once

// Thanks CitizenFX!
// Adapted by MoneyWasted for ChaosModV
// https://github.com/citizenfx/fivem/blob/master/code/components/extra-natives-five/src/Draw2dNatives.cpp

// This file is manually formatted.
// clang-format off

#define MAX_LINES 5000

namespace Memory
{
	inline void DrawLine(float x1, float y1, float x2, float y2, float width, int r, int g, int b, int a)
	{
		static char *(*AllocateDrawRect)(void *);
		static void (*SetDrawRectCoords)(void *, float, float, float, float);

		static char *drawRects           = 0;
		static int drawRectsSize         = 0;
		static int *mainThreadFrameIndex = 0;

		if (!drawRects)
		{
			// Get our base handle at the beginning of the DRAW_RECT function.
			Handle handle = Memory::FindPattern("48 8B C4 48 89 58 08 57 48 83 EC 70 48 63");
			if (!handle.IsValid())
			{
				return;
			}

			AllocateDrawRect     = handle.At(0x56).Into().Get<char *(void *)>();
			SetDrawRectCoords    = handle.At(0x92).Into().Get<void(void *, float, float, float, float)>();

			drawRects            = handle.At(0x32).Into().Get<char>();
			drawRectsSize        = handle.At(0x2C).Value<int>();
			mainThreadFrameIndex = handle.At(0x0E).Into().Get<int>();

			// At the instruction 'cmp edx, 500' in AllocateDrawRect.
			handle = Memory::FindPattern("81 FA F4 01 00 00 73 13");
			if (!handle.IsValid())
			{
				return;
			}

			// Increase the number of lines that can be drawn.
			Memory::Write<int>(handle.At(2).Get<int>(), MAX_LINES);
		}

		// Analysis in IDA shows that the return value of AllocateDrawRect would be a structure.
		if (auto rect = AllocateDrawRect(&drawRects[drawRectsSize * *mainThreadFrameIndex]))
		{
			// Clamp each color value then combine them.
			uint32_t color = (
				(std::clamp(a, 0, 255) << 24) |
				(std::clamp(r, 0, 255) << 16) |
				(std::clamp(g, 0, 255) << 8)  |
				(std::clamp(b, 0, 255) << 0)
			);

			SetDrawRectCoords(rect, x1, y1, x2, y2);

			// Set unknown structure pointers.
			*(uint32_t *)(rect + 0x34) &= 0xFA;
			*(uint32_t *)(rect + 0x34) |= 0x8A;
			*(float *)(rect + 0x1C)    = width;
			*(uint32_t *)(rect + 0x28) = color;
		}
	}

	inline void DrawLine(ChaosVector2 firstPos, ChaosVector2 secondPos, Color color, float thickness)
	{
		DrawLine(firstPos.x, firstPos.y, secondPos.x, secondPos.y, thickness, color.R, color.G, color.B, color.A);
	}
}