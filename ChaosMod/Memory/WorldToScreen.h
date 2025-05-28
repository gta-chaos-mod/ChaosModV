#pragma once

namespace Memory
{
	inline bool WorldToScreen(ChaosVector3 worldPosition, ChaosVector2 *screenPosition)
	{
		static bool (*WorldToScreen)(ChaosVector3, float *, float *);

		if (!WorldToScreen)
		{
			Handle handle = Memory::FindPattern("48 89 5C 24 ? 55 56 57 48 83 EC 70 65 4C 8B 0C 25", "41 56 56 57 53 48 83 EC 78 0F 29 7C 24 60 0F 29 74 24 50 4D");
			if (!handle.IsValid())
				return false;

			WorldToScreen = handle.Get<bool(ChaosVector3, float *, float *)>();
		}

		if (WorldToScreen(worldPosition, &screenPosition->x, &screenPosition->y))
			return true;

		return false;
	}
}