#pragma once

#include <unordered_map>

enum class EffectGroupType
{
	DEFAULT,
	TELEPORT,         // Effects which teleport the player potentially far away without (explicitly) being helpful
	SPAWN_GENERIC,    // Effects which spawn "generic" objects (like vehicles or props)
	SPAWN_ENEMY,      // Effects which spawn enemy peds
	SPAWN_COMPANION,  // Effects which spawn friendly peds
	WEAPONS,          // Effects which give/remove weapons
	PLAYERKILL,       // Effects which are (almost) guaranteed to immediately kill the player under any circumstance
	TRAFFIC_COLOR,    // Effects which change the color of traffic
	TIME_CHANGE,      // Effects which change time of day
	WEATHER_CHANGE,   // Effects which change the weather
};

struct EffectGroup
{
	int WeightMult = 1;
};

inline const std::unordered_map<EffectGroupType, EffectGroup> g_effectGroups
{
	{EffectGroupType::TELEPORT, {}},
	{EffectGroupType::SPAWN_GENERIC, { .WeightMult = 3 }},
	{EffectGroupType::SPAWN_ENEMY, { .WeightMult = 3 }},
	{EffectGroupType::SPAWN_COMPANION, {.WeightMult = 2 }},
	{EffectGroupType::WEAPONS, { .WeightMult = 2 }},
	{EffectGroupType::PLAYERKILL, {}},
	{EffectGroupType::TRAFFIC_COLOR, {.WeightMult = 2 }},
	{EffectGroupType::TIME_CHANGE, {}},
	{EffectGroupType::WEATHER_CHANGE, {}},
};

inline std::unordered_map<EffectGroupType, int> g_effectGroupMemberCount;