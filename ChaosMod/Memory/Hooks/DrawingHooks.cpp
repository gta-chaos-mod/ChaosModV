#include <stdafx.h>

#include "Hook.h"

// Thanks CitizenFX!
// Adapted by MoneyWasted for ChaosModV
// https://github.com/citizenfx/fivem/blob/master/code/components/extra-natives-five/src/Draw2dNatives.cpp

// Notes:
// Analysis in IDA shows that the return values of
// _HK_allocateDrawRect is most likely a structure
// of some sorts.

static char *g_drawRects           = 0;
static int *g_mainThreadFrameIndex = 0;
static int g_drawRectsSize         = 0;

char* (*_OG_allocateDrawRect)(void *);
char* _HK_allocateDrawRect(void *drawRects)
{
	return _OG_allocateDrawRect(drawRects);
}

void (*_OG_setDrawRectCoords)(void*, float, float, float, float);
void _HK_setDrawRectCoords(void *rect, float a2, float a3, float a4, float a5)
{
	_OG_setDrawRectCoords(rect, a2, a3, a4, a5);
}

static bool OnHook()
{
	// Get our base handle at the beginning of the DRAW_RECT function.
	Handle handle = Memory::FindPattern("48 8B C4 48 89 58 08 57 48 83 EC 70 48 63");
	if (!handle.IsValid())
	{
		return false;
	}

	g_drawRects            = handle.At(0x32).Into().Get<char>();
	g_drawRectsSize        = *handle.At(0x2C).Get<int>();
	g_mainThreadFrameIndex = handle.At(0x0E).Into().Get<int>();

	Memory::AddHook(handle.At(0x56).Into().Get<char>(), _HK_allocateDrawRect, &_OG_allocateDrawRect);
	Memory::AddHook(handle.At(0x92).Into().Get<void>(), _HK_setDrawRectCoords, &_OG_setDrawRectCoords);

	return true;
}

static RegisterHook registerHook(OnHook, nullptr, "_DrawingHooks");

namespace Hooks
{
	void DrawLine(float x1, float y1, float x2, float y2, float width, int r, int g, int b, int a)
	{
		if (auto rect = _HK_allocateDrawRect(&g_drawRects[g_drawRectsSize * *g_mainThreadFrameIndex]))
		{
			// Keep the formatting nice!
			// clang-format off
			uint32_t color = (
				(std::clamp(a, 0, 255) << 24) |
				(std::clamp(r, 0, 255) << 16) |
				(std::clamp(g, 0, 255) << 8)  |
				(std::clamp(b, 0, 255) << 0)
			);

			_HK_setDrawRectCoords(rect, x1, y1, x2, y2);

			// Unknown structure pointers.
			*(uint32_t*)(rect + 0x34) &= 0xFA;
			*(uint32_t*)(rect + 0x34) |= 0x8A;
			*(float*)(rect + 0x1C)    = width;
			*(uint32_t*)(rect + 0x28) = color;
		}
	}
}