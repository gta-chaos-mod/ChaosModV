#pragma once

#include "Handle.h"
#include "Memory.h"

namespace Memory
{
	inline void SetGravityLevel(float gravity)
	{
		static auto handle =
		    FindPattern("48 83 EC 28 83 F9 03 77", "48 83 EC 58 0F 29 7C 24 40 0F 29 74 24 30 83 F9 03");
		if (!handle.IsValid())
			return;
		static auto CPhysics__SetGravityLevel = handle.Get<void(unsigned int)>();
		static auto afGravityLevels           = handle.At(IsLegacy() ? 14 : 27).Into().Get<float>();

		auto ogLevel                          = afGravityLevels[0];
		afGravityLevels[0]                    = gravity;
		CPhysics__SetGravityLevel(0);
		afGravityLevels[0] = ogLevel;
	}
}