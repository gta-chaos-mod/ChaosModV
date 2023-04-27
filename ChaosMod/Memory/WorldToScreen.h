#pragma once

#include "Util/Types.h"

bool (*OG_WorldToScreen)(ChaosVector3, float *, float *);

static void *ms_WorldToScreenAddr = 0;

namespace Hooks
{
	inline bool WorldToScreen(Vector3 worldPosition, Vector3 *screenPosition)
	{
		static void *ms_WorldToScreenAddr = 0;

		if (!ms_WorldToScreenAddr)
		{
			Handle handle = Memory::FindPattern("48 89 5C 24 ? 55 56 57 48 83 EC 70 65 4C 8B 0C 25");
			if (!handle.IsValid())
			{
				return false;
			}

			ms_WorldToScreenAddr = handle.Get<void>();
			OG_WorldToScreen     = *(bool (*)(ChaosVector3, float *, float *))ms_WorldToScreenAddr;
		}

		ChaosVector3 _ {
			.x = worldPosition.x,
			.y = worldPosition.y,
			.z = worldPosition.z,
		};

		if (OG_WorldToScreen(_, &screenPosition->x, &screenPosition->y))
		{
			return true;
		}

		return false;
	}
}