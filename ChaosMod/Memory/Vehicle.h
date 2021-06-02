#pragma once

#include "Memory.h"
#include "Handle.h"

#include "../Util/Natives.h"

#include <vector>

using DWORD64 = unsigned long long;
using WORD = unsigned short;

using Hash = unsigned long;
using Vehicle = int;

namespace Memory
{
	inline std::vector<Hash> GetAllVehModels()
	{
		static std::vector<Hash> c_rgVehModels;

		if (c_rgVehModels.empty())
		{
			Handle handle;

			handle = FindPattern("48 8B 05 ?? ?? ?? ?? 48 8B 14 D0 EB 0D 44 3B 12");
			if (!handle.IsValid())
			{
				return c_rgVehModels;
			}

			handle = handle.At(2).Into();
			DWORD64 ullModelList = handle.Value<DWORD64>();

			handle = FindPattern("0F B7 05 ?? ?? ?? ?? 44 8B 49 18 45 33 D2 48 8B F1");
			if (!handle.IsValid())
			{
				return c_rgVehModels;
			}

			handle = handle.At(2).Into();
			WORD usMaxModels = handle.Value<WORD>();

			for (WORD usIdx = 0; usIdx < usMaxModels; usIdx++)
			{
				DWORD64 ullEntry = *reinterpret_cast<DWORD64*>(ullModelList + 8 * usIdx);
				if (!ullEntry)
				{
					continue;
				}

				Hash ulModel = *reinterpret_cast<Hash*>(ullEntry);

				if (IS_MODEL_VALID(ulModel) && IS_MODEL_A_VEHICLE(ulModel))
				{
					c_rgVehModels.push_back(ulModel);
				}
			}
		}

		return c_rgVehModels;
	}

	inline void SetVehicleOutOfControl(Vehicle vehicle, bool bState)
	{
		static __int64(*sub_7FF69C749B98)(int a1);

		static bool bIsSetup = false;
		if (!bIsSetup)
		{
			Handle handle;

			handle = FindPattern("E8 ? ? ? ? 8D 53 01 33 DB");
			if (!handle.IsValid())
			{
				return;
			}

			sub_7FF69C749B98 = handle.Into().Get<__int64(int)>();

			bIsSetup = true;
		}

		int iVehClass = GET_VEHICLE_CLASS(vehicle);
		
		static const Hash c_ulBlimpHash = GET_HASH_KEY("BLIMP");
		Hash ulVehModel = GET_ENTITY_MODEL(vehicle);
		if (iVehClass == 15 || iVehClass == 16 || ulVehModel == c_ulBlimpHash) // No helis or planes, also make sure to explicitely exclude blimps at all costs as they cause a crash
		{
			return;
		}

		__int64 v6 = sub_7FF69C749B98(vehicle);
		if (v6)
		{
			v6 = (*reinterpret_cast<__int64(**)(__int64)>(*reinterpret_cast<__int64*>(v6) + 1528))(v6);

			if (v6)
			{
				*reinterpret_cast<BYTE*>(v6 + 2373) &= 0xFEu;
				*reinterpret_cast<BYTE*>(v6 + 2373) |= bState;
			}
		}
	}

	inline void OverrideVehicleHeadlightColor(int iIdx, bool bOverrideColor, int r, int g, int b)
	{
		static DWORD64* qword_7FF69E1E8E88 = nullptr;

		static const int c_iMaxColors = 13;
		static DWORD c_ulOrigColors[c_iMaxColors] = { };

		if (iIdx >= c_iMaxColors)
		{
			return;
		}

		if (!qword_7FF69E1E8E88)
		{
			Handle handle;

			handle = FindPattern("48 89 0D ? ? ? ? E8 ? ? ? ? 48 8D 4D C8 E8 ? ? ? ? 48 8D 15 ? ? ? ? 48 8D 4D C8 45 33 C0 E8 ? ? ? ? 4C 8D 0D");
			if (!handle.IsValid())
			{
				return;
			}

			qword_7FF69E1E8E88 = handle.At(2).Into().Get<DWORD64>();
		}

		DWORD* pulColors = *reinterpret_cast<DWORD**>(*qword_7FF69E1E8E88 + 328);

		if (!c_ulOrigColors[0])
		{
			// Orig colors not backed up yet, do it now

			for (int i = 0; i < c_iMaxColors; i++)
			{
				c_ulOrigColors[i] = pulColors[i * 4];
			}
		}

		DWORD ulNewColor = ((((r << 24) | (g << 16)) | b << 8) | 0xFF);

		pulColors[iIdx * 4] = bOverrideColor ? ulNewColor : c_ulOrigColors[iIdx];
		pulColors[iIdx * 4 + 1] = bOverrideColor ? ulNewColor : c_ulOrigColors[iIdx];
	}

	inline bool IsVehicleBraking(Vehicle vehicle)
	{
		static __int64(*sub_7FF788D32A60)(Vehicle vehicle) = nullptr;

		if (!sub_7FF788D32A60)
		{
			Handle handle = FindPattern("E8 ? ? ? ? 48 85 FF 74 47");
			if (!handle.IsValid())
			{
				return false;
			}

			sub_7FF788D32A60 = handle.Into().Get<__int64(Vehicle)>();
		}

		__int64 result = sub_7FF788D32A60(vehicle);

		return result ? *reinterpret_cast<float*>(result + 2496) : false;
	}
}