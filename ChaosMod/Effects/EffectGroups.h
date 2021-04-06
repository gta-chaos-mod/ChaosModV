#pragma once

#include <unordered_map>

enum class EffectGroupType
{
	NONE,
	TELEPORT,            // Effects which teleport the player potentially far away without (explicitly) being helpful
	SPAWN_GENERIC,       // Effects which spawn "generic" objects (like vehicles or props)
	SPAWN_ENEMY_SPECIAL, // Effects which spawn "special" enemy peds, DO NOT ADD ANY ADDITIONAL EFFECTS TO THIS GROUP
	SPAWN_ENEMY,         // Effects which spawn enemy peds
	SPAWN_COMPANION,     // Effects which spawn friendly peds
	WEAPONS,             // Effects which give/remove weapons
	PLAYERKILL,          // Effects which are (almost) guaranteed to immediately kill the player under any circumstance
	TRAFFIC_COLOR,       // Effects which change the color of traffic
	TIME_CHANGE,         // Effects which change time of day
	WEATHER_CHANGE,      // Effects which change the weather
};

struct EffectGroup
{
	int WeightMult = 1;
};

inline const std::unordered_map<EffectGroupType, EffectGroup> g_effectGroups
{
	{EffectGroupType::TELEPORT, {}},
	{EffectGroupType::SPAWN_GENERIC, { .WeightMult = 5 }},
	{EffectGroupType::SPAWN_ENEMY_SPECIAL, {.WeightMult = 5 }},
	{EffectGroupType::SPAWN_ENEMY, { .WeightMult = 4 }},
	{EffectGroupType::SPAWN_COMPANION, {.WeightMult = 5 }},
	{EffectGroupType::WEAPONS, { .WeightMult = 4 }},
	{EffectGroupType::PLAYERKILL, {}},
	{EffectGroupType::TRAFFIC_COLOR, { .WeightMult = 3 }},
	{EffectGroupType::TIME_CHANGE, {}},
	{EffectGroupType::WEATHER_CHANGE, {}},
};

inline std::unordered_map<EffectGroupType, int> g_allEffectGroupMemberCount;
inline std::unordered_map<EffectGroupType, int> g_currentEffectGroupMemberCount;