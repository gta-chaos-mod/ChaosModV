#pragma once

#include <unordered_map>

enum class EEffectGroupType
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
	TrafficSpawner,	 // Effects which change or spawn a lot of vehicles
	TimeChange,         // Effects which change time of day
	WeatherChange,      // Effects which change the weather
};

struct EffectGroup
{
	int WeightMult = 1;
};

inline const std::unordered_map<EEffectGroupType, EffectGroup> g_EffectGroups
{
	{EEffectGroupType::Teleport, {.WeightMult = 2 }},
	{EEffectGroupType::SpawnGeneric, { .WeightMult = 5 }},
	{EEffectGroupType::SpawnEnemySpecial, {.WeightMult = 5 }},
	{EEffectGroupType::SpawnEnemy, { .WeightMult = 4 }},
	{EEffectGroupType::SpawnCompanion, {.WeightMult = 5 }},
	{EEffectGroupType::Weapons, { .WeightMult = 4 }},
	{EEffectGroupType::PlayerKill, { .WeightMult = 2 }},
	{EEffectGroupType::TrafficSpawner, {.WeightMult = 3 }},
	{EEffectGroupType::TrafficColor, { .WeightMult = 3 }},
	{EEffectGroupType::TimeChange, {.WeightMult = 2 }},
	{EEffectGroupType::WeatherChange, {.WeightMult = 2 }},
};

inline std::unordered_map<EEffectGroupType, int> g_dictAllEffectGroupMemberCount;
inline std::unordered_map<EEffectGroupType, int> g_dictCurrentEffectGroupMemberCount;