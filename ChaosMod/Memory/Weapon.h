#pragma once

#include "Memory.h"
#include "Handle.h"
#include "Entity.h"

#include "../Util/Natives.h"

#include <vector>

namespace Memory
{
	inline uintptr_t GetPedWeaponInfo(Ped ulPed)
	{
		auto baseAddr = GetScriptHandleBaseAddress(ulPed);
		return *reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(baseAddr + 0x10D8) + 0x20);
	}

	inline bool IsWeaponRangable(Hash ulWepHash)
	{
		std::vector<Hash> weps = //Basically anything throwable or projectile.
		{
			2726580491,
			1305664598,
			2982836145,
			1752584910,
			2481070269,
			741814745,
			4256991824,
			2694266206,
			615608432,
			883325847,
			1672152130,
			2138347493,
			125959754
		};

		if (std::count(weps.begin(), weps.end(), ulWepHash))
			return false;
		return true;
	}

	inline void SetPedWeaponRange(Ped ulPed, float fRange)
	{
		auto infoAddr = GetPedWeaponInfo(ulPed);
		*reinterpret_cast<float*>(infoAddr + 0x28C) = fRange;
	}

	inline float GetPedWeaponRange(Ped ulPed)
	{
		auto infoAddr = GetPedWeaponInfo(ulPed);
		return *reinterpret_cast<float*>(infoAddr + 0x28C);
	}

	inline void SetPedWeaponBulletPerBatch(Ped ulPed, int fBullets)
	{
		auto infoAddr = GetPedWeaponInfo(ulPed);
		*reinterpret_cast<uint32_t*>(infoAddr + 0x120) = fBullets;
	}

	inline uint32_t GetPedWeaponBulletPerBatch(Ped ulPed)
	{ 
		auto infoAddr = GetPedWeaponInfo(ulPed);
		return *reinterpret_cast<uint32_t*>(infoAddr + 0x120);
	}

	inline void SetPedWeaponSpread(Ped ulPed, float fSpread)
	{
		auto infoAddr = GetPedWeaponInfo(ulPed);
		*reinterpret_cast<float*>(infoAddr + 0x0124) = fSpread;
	}

	inline float GetPedWeaponSpread(Ped ulPed)
	{
		auto infoAddr = GetPedWeaponInfo(ulPed);
		return *reinterpret_cast<float*>(infoAddr + 0x0124);
	}

	inline void SetPedWeaponForce(Ped ulPed, float fForce)
	{
		auto infoAddr = GetPedWeaponInfo(ulPed);
		*reinterpret_cast<float*>(infoAddr + 0x00D8) = fForce;
	}

	inline float GetPedWeaponForce(Ped ulPed)
	{
		auto infoAddr = GetPedWeaponInfo(ulPed);
		return *reinterpret_cast<float*>(infoAddr + 0x00D8);
	}

	inline bool IsPedWeaponShotgun(Ped ulPed)
	{
		auto infoAddr = GetPedWeaponInfo(ulPed);
		return *reinterpret_cast<int32_t*>(infoAddr + 0x0058) == 5;
	}
}