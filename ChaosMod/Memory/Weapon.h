#pragma once

#include "Memory.h"
#include "Handle.h"
#include "Entity.h"

#include "../Util/Natives.h"

#include "Util/Peds.h"

#include <vector>

namespace Memory
{
	inline uintptr_t GetWeaponInfo(Hash ulHash)
	{
		REQUEST_WEAPON_ASSET(ulHash, 31, 26);
		Ped temp      = CreateTempPed(0x705E61F2, 0, 0, 0, 0);

		GIVE_WEAPON_TO_PED(temp, ulHash, 9999, false, true);
		SET_CURRENT_PED_WEAPON(temp, ulHash, true);
		
		auto baseAddr = GetScriptHandleBaseAddress(temp);
		return *reinterpret_cast<uintptr_t *>(*reinterpret_cast<uintptr_t *>(baseAddr + 0x10B8) + 0x20);
	}

	inline bool IsWeaponRangable(Hash ulWepHash)
	{
		std::vector<Hash> weps = //Basically anything throwable or projectile.
		{
			2726580491,
			2982836145,
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
			125959754,
			3676729658,
			1834241177,
			600439132,
			1233104067,
			2874559379,
			126349499,
			3125143736
		};

		if (std::count(weps.begin(), weps.end(), ulWepHash))
			return false;
		return true;
	}

	inline void SetWeaponDamage(Hash ulHash, float fDamage)
	{
		auto infoAddr = GetWeaponInfo(ulHash);
		*reinterpret_cast<float *>(infoAddr + 0xB0) = fDamage;
	}

	inline float GetWeaponDamage(Hash ulHash)
	{
		auto infoAddr = GetWeaponInfo(ulHash);
		return *reinterpret_cast<float*>(infoAddr + 0xB0);
	}	
	
	inline void SetWeaponRange(Hash ulHash, float fRange)
	{
		auto infoAddr = GetWeaponInfo(ulHash);
		*reinterpret_cast<float*>(infoAddr + 0x28C) = fRange;
	}

	inline float GetWeaponRange(Hash ulHash)
	{
		auto infoAddr = GetWeaponInfo(ulHash);
		return *reinterpret_cast<float*>(infoAddr + 0x28C);
	}

	inline void SetWeaponBulletsPerBatch(Hash ulHash, int iBullets)
	{
		auto infoAddr = GetWeaponInfo(ulHash);
		*reinterpret_cast<uint32_t *>(infoAddr + 0x120) = iBullets;
	}

	inline uint32_t GetWeaponBulletsPerBatch(Hash ulHash)
	{ 
		auto infoAddr = GetWeaponInfo(ulHash);
		return *reinterpret_cast<uint32_t*>(infoAddr + 0x120);
	}

	inline void SetWeaponSpread(Hash ulHash, float fSpread)
	{
		auto infoAddr = GetWeaponInfo(ulHash);
		*reinterpret_cast<float*>(infoAddr + 0x0124) = fSpread;
	}

	inline float GetWeaponSpread(Hash ulHash)
	{
		auto infoAddr = GetWeaponInfo(ulHash);
		return *reinterpret_cast<float*>(infoAddr + 0x0124);
	}

	inline void SetWeaponForce(Hash ulHash, float fForce)
	{
		auto infoAddr = GetWeaponInfo(ulHash);
		*reinterpret_cast<float*>(infoAddr + 0x00D8) = fForce;
	}

	inline float GetWeaponForce(Hash ulHash)
	{
		auto infoAddr = GetWeaponInfo(ulHash);
		return *reinterpret_cast<float*>(infoAddr + 0x00D8);
	}

	inline bool IsWeaponShotgun(Hash ulHash)
	{
		auto infoAddr = GetWeaponInfo(ulHash);
		return *reinterpret_cast<int32_t*>(infoAddr + 0x0058) == 5;
	}
}
