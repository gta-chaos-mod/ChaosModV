#include <stdafx.h>

#include "Hook.h"
#include "Util/Types.h"
#include "WorldToScreenHook.h"

static Resolution *m_resolution;

bool (*_OG_WorldToScreen)(Vec3 worldPosition, float *X, float *Y);
bool _HK_WorldToScreen(Vec3 worldPosition, float *X, float *Y)
{
	return _OG_WorldToScreen(worldPosition, X, Y);
}

static bool OnHook()
{
	Handle handle = Memory::FindPattern("48 89 5C 24 ? 55 56 57 48 83 EC 70 65 4C 8B 0C 25");
	if (!handle.IsValid())
	{
		return false;
	}

	Memory::AddHook(handle.Get<bool>(), _HK_WorldToScreen, &_OG_WorldToScreen);

	handle = Memory::FindPattern("8B 0D ? ? ? ? 49 8D 56 28");
	if (!handle.IsValid())
	{
		return false;
	}

	m_resolution = handle.At(1).Into().Get<Resolution>();

	return true;
}

static RegisterHook registerHook(OnHook, nullptr, "_WorldToScreen");

namespace Hooks
{
	bool WorldToScreen(Vector3 worldPosition, Vec2 *screenPosition)
	{
		Vec3 worldVec { .x = worldPosition.x, .y = worldPosition.y, .z = worldPosition.z };

		if (_HK_WorldToScreen(worldVec, &screenPosition->x, &screenPosition->y))
		{
			return true;
		}

		return false;
	}
}