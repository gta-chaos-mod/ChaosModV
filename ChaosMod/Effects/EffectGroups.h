#pragma once

#include <unordered_map>

enum class EffectGroupType
{
	None,
	Teleport,            // Effects which teleport the player potentially far away without (explicitly) being helpful
	SpawnGeneric,       // Effects which spawn "generic" objects (like vehicles or props)
	SpawnEnemySpecial, // Effects which spawn "special" enemy peds, DO NOT ADD ANY ADDITIONAL EFFECTS TO THIS GROUP
	SpawnEnemy,         // Effects which spawn enemy peds
	SpawnCompanion,     // Effects which spawn friendly peds
	Weapons,             // Effects which give/remove weapons
	PlayerKill,          // Effects which are (almost) guaranteed to immediately kill the player under any circumstance
	TrafficColor,       // Effects which change the color of traffic
	TimeChange,         // Effects which change time of day
	WeatherChange,      // Effects which change the weather
};

struct EffectGroup
{
	int WeightMult = 1;
};

inline const std::unordered_map<EffectGroupType, EffectGroup> g_EffectGroups
{
	{EffectGroupType::Teleport, {.WeightMult = 2 }},
	{EffectGroupType::SpawnGeneric, { .WeightMult = 5 }},
	{EffectGroupType::SpawnEnemySpecial, {.WeightMult = 5 }},
	{EffectGroupType::SpawnEnemy, { .WeightMult = 4 }},
	{EffectGroupType::SpawnCompanion, {.WeightMult = 5 }},
	{EffectGroupType::Weapons, { .WeightMult = 4 }},
	{EffectGroupType::PlayerKill, { .WeightMult = 2 }},
	{EffectGroupType::TrafficColor, { .WeightMult = 3 }},
	{EffectGroupType::TimeChange, {.WeightMult = 2 }},
	{EffectGroupType::WeatherChange, {.WeightMult = 2 }},
};

inline std::unordered_map<EffectGroupType, int> g_AllEffectGroupMemberCount;
inline std::unordered_map<EffectGroupType, int> g_CurrentEffectGroupMemberCount;