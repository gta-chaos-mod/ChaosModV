#pragma once

#include "Memory.h"
#include "../nativesNoNamespaces.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <vector>
#include <fstream>

namespace Memory
{
	inline std::vector<Hash> GetAllVehModels()
	{
		static std::vector<Hash> vehModels;

		if (vehModels.empty())
		{
			Handle handle = FindPattern("48 8B 05 ?? ?? ?? ?? 48 8B 14 D0 EB 0D 44 3B 12");
			if (!handle.IsValid())
			{
				return vehModels;
			}

			handle = handle.At(2).Into();
			DWORD64 modelList = handle.Value<DWORD64>();

			handle = FindPattern("0F B7 05 ?? ?? ?? ?? 44 8B 49 18 45 33 D2 48 8B F1");
			if (!handle.IsValid())
			{
				return vehModels;
			}

			handle = handle.At(2).Into();
			WORD maxModels = handle.Value<WORD>();

			for (WORD i = 0; i < maxModels; i++)
			{
				DWORD64 entry = *reinterpret_cast<DWORD64*>(modelList + 8 * i);
				if (!entry)
				{
					continue;
				}

				Hash model = *reinterpret_cast<Hash*>(entry);

				if (IS_MODEL_VALID(model) && IS_MODEL_A_VEHICLE(model))
				{
					vehModels.push_back(model);
				}
			}
		}

		return vehModels;
	}
}