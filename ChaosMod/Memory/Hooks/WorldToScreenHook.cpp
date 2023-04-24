#include <stdafx.h>

#include "Hook.h"
#include "Util/Types.h"
#include "WorldToScreenHook.h"

bool (*OG_WorldToScreen)(rage::Vector3, float *, float *);

static void **ms_WorldToScreenAddr = 0;

static bool OnHook()
{
	Handle handle = Memory::FindPattern("48 89 5C 24 ? 55 56 57 48 83 EC 70 65 4C 8B 0C 25");
	if (!handle.IsValid())
	{
		return false;
	}

	ms_WorldToScreenAddr = handle.Get<void *>();
	OG_WorldToScreen     = *(bool (*)(rage::Vector3, float *, float *))ms_WorldToScreenAddr;

	return true;
}

static RegisterHook registerHook(OnHook, nullptr, "_WorldToScreen");

namespace Hooks
{
	bool WorldToScreen(Vector3 worldPosition, rage::Vector2 *screenPosition)
	{
		rage::Vector3 worldVec { .x = worldPosition.x, .y = worldPosition.y, .z = worldPosition.z };

		if (OG_WorldToScreen(worldVec, &screenPosition->x, &screenPosition->y))
		{
			return true;
		}

		return false;
	}
}