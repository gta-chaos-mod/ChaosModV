#pragma once

using Hash = unsigned long;

namespace Util
{
	// TODO: Maybe CWeaponInfo has some field which can be checked (instead of hardcoding the weapon hashes)
	inline bool IsWeaponShotgun(Hash ulWepHash)
	{
		switch (ulWepHash)
		{
		case 487013001:
		case 2017895192:
		case -1654528753:
		case -494615257:
		case -1466123874:
		case 984333226:
		case -275439685:
		case 317205821:
			return true;
		}

		return false;
	}
}