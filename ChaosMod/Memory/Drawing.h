#pragma once

#include <cstdint>

// Thanks CitizenFX!
// Adapted by MoneyWasted for ChaosModV
// https://github.com/citizenfx/fivem/blob/master/code/components/extra-natives-five/src/Draw2dNatives.cpp

#define MAX_LINES 5000

namespace Memory
{
	inline void DrawLine(float x1, float y1, float x2, float y2, float width, std::uint32_t color)
	{
		static char *(*AllocateDrawRect)(void *);
		static void (*SetDrawRectCoords)(void *, float, float, float, float);

		static char *drawRects           = 0;
		static int drawRectsSize         = 0;
		static int *mainThreadFrameIndex = 0;

		if (!drawRects)
		{
			if (IsLegacy())
			{
				// Get our base handle at the beginning of the DRAW_RECT function.
				Handle handle = Memory::FindPattern("48 8B C4 48 89 58 08 57 48 83 EC 70 48 63");
				if (!handle.IsValid())
					return;

				AllocateDrawRect     = handle.At(0x56).Into().Get<char *(void *)>();

				auto offsetSDC       = getGameVersion() < eGameVersion::VER_1_0_3407_0 ? 0x92 : 0x97;
				SetDrawRectCoords    = handle.At(offsetSDC).Into().Get<void(void *, float, float, float, float)>();

				drawRects            = handle.At(0x32).Into().Get<char>();
				drawRectsSize        = handle.At(0x2C).Value<int>();
				mainThreadFrameIndex = handle.At(0x0E).Into().Get<int>();
			}
			else
			{
				Handle handle = Memory::FindPattern("44 0F 28 C8 48 63 05");
				if (!handle.IsValid())
					return;

				AllocateDrawRect     = handle.At(0x1C).Into().Get<char *(void *)>();
				drawRects            = handle.At(0x14).Into().Get<char>();
				drawRectsSize        = handle.At(0x0D).Value<int>();
				mainThreadFrameIndex = handle.At(0x06).Into().Get<int>();

				handle               = Memory::FindPattern("E8 ?? ?? ?? ?? B9 70 FF FF FF");
				if (!handle.IsValid())
					return;

				SetDrawRectCoords = handle.Into().Get<void(void *, float, float, float, float)>();
			}

			// At the instruction 'cmp edx, 500' in AllocateDrawRect.
			Handle handle = Memory::FindPattern("81 FA F4 01 00 00 73 13", "81 FA ?? ?? ?? ?? 77 14 48 8D 04 52");
			if (!handle.IsValid())
				return;

			// Increase the number of lines that can be drawn.
			Memory::Write<int>(handle.At(2).Get<int>(), MAX_LINES);
		}

		// Analysis in IDA shows that the return value of AllocateDrawRect would be a structure.
		if (auto rect = AllocateDrawRect(&drawRects[drawRectsSize * *mainThreadFrameIndex]))
		{
			SetDrawRectCoords(rect, x1, y1, x2, y2);

			// Set unknown structure pointers.
			*(std::uint32_t *)(rect + (IsLegacy() ? 0x34 : 0x48)) &= 0xFA;
			*(std::uint32_t *)(rect + (IsLegacy() ? 0x34 : 0x48)) |= 0x8A;
			*(float *)(rect + (IsLegacy() ? 0x1C : 0x30))         = width;
			*(std::uint32_t *)(rect + (IsLegacy() ? 0x28 : 0x3C)) = color;
		}
	}

	inline void DrawLine(ChaosVector2 firstPos, ChaosVector2 secondPos, Color &color, float thickness)
	{
		DrawLine(firstPos.x, firstPos.y, secondPos.x, secondPos.y, thickness, color.Get());
	}
}