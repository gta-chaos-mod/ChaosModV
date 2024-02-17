#pragma once

#include <unordered_map>

using Hash = unsigned long;

namespace Util
{
	inline bool IsWeaponShotgun(Hash weaponHash)
	{
		static std::unordered_map<Hash, bool> cachedResults;
		static const Hash shotgunGroup = "GROUP_SHOTGUN"_hash;

		if (const auto result = cachedResults.find(weaponHash); result != cachedResults.end())
		{
			return result->second;
		}

		return cachedResults.emplace(weaponHash, (GET_WEAPONTYPE_GROUP(weaponHash) == shotgunGroup)).first->second;
	}
}