#pragma once

#include "Memory.h"
#include "Handle.h"
#include "nativesNoNamespaces.h"

#include <vector>

typedef unsigned short WORD;
typedef unsigned long long DWORD64;
typedef unsigned long Hash;
typedef int Vehicle;

namespace Memory
{
	inline std::vector<Hash> GetAllVehModels()
	{
		static std::vector<Hash> vehModels;

		if (vehModels.empty())
		{
			Handle handle;

			handle = FindPattern("48 8B 05 ?? ?? ?? ?? 48 8B 14 D0 EB 0D 44 3B 12");
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

	inline void SetVehicleOutOfControl(Vehicle vehicle, bool state)
	{
		static __int64(*sub_7FF69C749B98)(int a1);

		static bool isSetup = false;
		if (!isSetup)
		{
			Handle handle;

			handle = FindPattern("E8 ? ? ? ? 8D 53 01 33 DB");
			if (!handle.IsValid())
			{
				return;
			}

			sub_7FF69C749B98 = handle.Into().Get<__int64(int)>();

			isSetup = true;
		}

		__int64 v6 = sub_7FF69C749B98(vehicle);
		if (v6)
		{
			v6 = (*reinterpret_cast<__int64(**)(__int64)>(*reinterpret_cast<__int64*>(v6) + 1528))(v6);

			*reinterpret_cast<BYTE*>(v6 + 2341) &= 0xFEu;
			*reinterpret_cast<BYTE*>(v6 + 2341) |= state;
		}
	}
}