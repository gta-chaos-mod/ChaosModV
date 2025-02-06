#pragma once

#include "Memory/Entity.h"
#include "Memory/Handle.h"
#include "Memory/Memory.h"

#include "Util/Hash.h"
#include "Util/Natives.h"

#include <unordered_set>
#include <vector>

using DWORD64 = unsigned long long;
using WORD    = unsigned short;

using Hash    = unsigned long;
using Vehicle = int;

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
				return vehModels;

			handle         = handle.At(2).Into();
			auto modelList = handle.Value<DWORD64>();

			handle         = FindPattern("0F B7 05 ?? ?? ?? ?? 44 8B 49 18 45 33 D2 48 8B F1");
			if (!handle.IsValid())
				return vehModels;

			handle         = handle.At(2).Into();
			auto maxModels = handle.Value<WORD>();

			//  Stub vehicles, thanks R* lol
			static const std::unordered_set<Hash> blacklistedModels {
				"arbitergt"_hash, "astron2"_hash, "cyclone2"_hash, "ignus2"_hash, "s95"_hash,
			};

			for (WORD i = 0; i < maxModels; i++)
			{
				auto entry = *reinterpret_cast<DWORD64 *>(modelList + 8 * i);
				if (!entry)
					continue;

				auto model = *reinterpret_cast<Hash *>(entry);

				if (IS_MODEL_VALID(model) && IS_MODEL_A_VEHICLE(model) && !blacklistedModels.contains(model))
					vehModels.push_back(model);
			}
		}

		return vehModels;
	}

	inline void SetVehicleOutOfControl(Vehicle vehicle, bool state)
	{
		int vehClass  = GET_VEHICLE_CLASS(vehicle);
		Hash vehModel = GET_ENTITY_MODEL(vehicle);
		if (vehClass == 15 || vehClass == 16
		    || vehModel == "BLIMP"_hash) // No helis or planes, also make sure to explicitely exclude blimps at all
		                                 // costs as they cause a crash
		{
			return;
		}

		static auto outOfControlStateOffset = []() -> WORD
		{
			auto handle = FindPattern("FF 90 ? ? 00 00 80 A3 ? ? 00 00 fE 40 80 E7 01");
			if (!handle.IsValid())
			{
				LOG("Vehicle out of control state offset not found!");
				return 0;
			}

			return handle.At(8).Value<std::uint16_t>();
		}();

		if (!outOfControlStateOffset)
			return;

		auto result = GetScriptHandleBaseAddress(vehicle);
		if (result)
		{
			*reinterpret_cast<std::uint8_t *>(result + outOfControlStateOffset) &= 0xFEu;
			*reinterpret_cast<bool *>(result + outOfControlStateOffset) |= state;
		}
	}

	inline void OverrideVehicleHeadlightColor(int idx, bool overrideColor, int r, int g, int b)
	{
		static std::uint64_t *qword_7FF69E1E8E88 = nullptr;

		static const int maxColors               = 13;
		static DWORD origColors[maxColors]       = {};

		if (idx >= maxColors)
			return;

		if (!qword_7FF69E1E8E88)
		{
			Handle handle;

			handle = FindPattern("48 89 0D ? ? ? ? E8 ? ? ? ? 48 8D 4D C8 E8 ? ? ? ? 48 8D 15 ? ? ? ? 48 8D 4D C8 45 "
			                     "33 C0 E8 ? ? ? ? 4C 8D 0D");
			if (!handle.IsValid())
				return;

			qword_7FF69E1E8E88 = handle.At(2).Into().Get<std::uint64_t>();
		}

		auto colors = *reinterpret_cast<DWORD **>(*qword_7FF69E1E8E88 + 328);

		if (!origColors[0])
		{
			// Orig colors not backed up yet, do it now

			for (int i = 0; i < maxColors; i++)
				origColors[i] = colors[i * 4];
		}

		DWORD newColor      = ((((r << 24) | (g << 16)) | b << 8) | 0xFF);

		colors[idx * 4]     = overrideColor ? newColor : origColors[idx];
		colors[idx * 4 + 1] = overrideColor ? newColor : origColors[idx];
	}

	inline bool IsVehicleBraking(Vehicle vehicle)
	{
		static auto brakeStateOffset = []() -> WORD
		{
			auto handle = FindPattern("F3 0F 11 80 ? ? 00 00 48 83 C4 20 5B C3 ? ? 40 53");
			if (!handle.IsValid())
			{
				LOG("Vehicle brake state offset not found!");
				return 0;
			}

			return handle.At(4).Value<std::uint16_t>();
		}();

		auto result = GetScriptHandleBaseAddress(vehicle);

		return result ? *reinterpret_cast<float *>(result + brakeStateOffset) : false;
	}

	inline Vector3 GetVector3(auto offset)
	{
		return Vector3(*reinterpret_cast<float *>(offset), *reinterpret_cast<float *>(offset + 0x4),
		               *reinterpret_cast<float *>(offset + 0x8));
	}

	inline void SetVector3(auto offset, const Vector3 &vec)
	{
		*reinterpret_cast<float *>(offset)       = vec.x;
		*reinterpret_cast<float *>(offset + 0x4) = vec.y;
		*reinterpret_cast<float *>(offset + 0x8) = vec.z;
	}

	inline void SetVehicleScale(Vehicle veh, float scaleMultiplier)
	{
		auto baseAddr = GetScriptHandleBaseAddress(veh);
		if (!baseAddr)
			return;

		auto passengerMatrixAddress = baseAddr + 0x60;
		Vector3 passengerForwardVec = Memory::GetVector3(passengerMatrixAddress + 0x00);
		Vector3 passengerRightVec   = Memory::GetVector3(passengerMatrixAddress + 0x10);
		Vector3 passengerUpVec      = Memory::GetVector3(passengerMatrixAddress + 0x20);

		auto vehicleMatrixAddress   = *reinterpret_cast<uintptr_t *>(baseAddr + 0x30) + 0x20;
		Vector3 vehicleForwardVec   = Memory::GetVector3(vehicleMatrixAddress + 0x00);
		Vector3 vehicleRightVec     = Memory::GetVector3(vehicleMatrixAddress + 0x10);
		Vector3 vehicleUpVec        = Memory::GetVector3(vehicleMatrixAddress + 0x20);

		Memory::SetVector3(passengerMatrixAddress + 0x00, passengerForwardVec * scaleMultiplier);
		Memory::SetVector3(passengerMatrixAddress + 0x10, passengerRightVec * scaleMultiplier);
		Memory::SetVector3(passengerMatrixAddress + 0x20, passengerUpVec * scaleMultiplier);

		Memory::SetVector3(vehicleMatrixAddress + 0x00, vehicleForwardVec * scaleMultiplier);
		Memory::SetVector3(vehicleMatrixAddress + 0x10, vehicleRightVec * scaleMultiplier);
		Memory::SetVector3(vehicleMatrixAddress + 0x20, vehicleUpVec * scaleMultiplier);
	}
}