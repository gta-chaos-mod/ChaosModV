#pragma once

using Hash = unsigned long;

namespace Util
{
	inline bool IsWeaponShotgun(Hash weaponHash)
	{
		return GET_WEAPONTYPE_GROUP(weaponHash) == "GROUP_SHOTGUN"_hash;
	}
}