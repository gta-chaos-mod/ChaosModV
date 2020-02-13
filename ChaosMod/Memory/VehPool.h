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
			auto handle = FindPattern("48 8B 05 ?? ?? ?? ?? 48 8B 14 D0 EB 0D 44 3B 12");
			if (!handle.IsValid())
			{
				return vehModels;
			}

			auto addr = handle.Addr();
			addr += 7 + *reinterpret_cast<DWORD*>(addr + 3);
			auto modelList = *reinterpret_cast<DWORD64*>(addr);

			handle = FindPattern("0F B7 05 ?? ?? ?? ?? 44 8B 49 18 45 33 D2 48 8B F1");
			if (!handle.IsValid())
			{
				return vehModels;
			}

			addr = handle.Addr();
			addr += 7 + *reinterpret_cast<DWORD*>(addr + 3);
			auto maxModels = *reinterpret_cast<WORD*>(addr);

			for (WORD i = 0; i < maxModels + 1; i++)
			{
				auto entry = *reinterpret_cast<DWORD64*>(modelList + 8 * i);
				if (!entry)
				{
					continue;
				}

				auto model = *reinterpret_cast<Hash*>(entry);

				if (IS_MODEL_VALID(model) && IS_MODEL_A_VEHICLE(model))
				{
					vehModels.push_back(model);
				}
			}
		}

		return vehModels;
	}
}