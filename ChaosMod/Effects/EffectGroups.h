#pragma once

#include <string>
#include <unordered_map>

enum class EEffectGroupType
{
	None,
	Teleport,          // Effects which teleport the player potentially far away without (explicitly) being helpful
	SpawnGeneric,      // Effects which spawn "generic" objects (like vehicles or props)
	SpawnEnemySpecial, // Effects which spawn "special" enemy peds, DO NOT ADD ANY ADDITIONAL EFFECTS TO THIS GROUP
	SpawnEnemy,        // Effects which spawn enemy peds
	SpawnCompanion,    // Effects which spawn friendly peds
	Weapons,           // Effects which give/remove weapons
	PlayerKill,        // Effects which are (almost) guaranteed to immediately kill the player under any circumstance
	TrafficColor,      // Effects which change the color of traffic
	TrafficSpawner,    // Effects which change or spawn a lot of vehicles
	TimeChange,        // Effects which change time of day
	WeatherChange,     // Effects which change the weather
	Shader             // Shader effects
};

struct EffectGroupData
{
	unsigned short WeightMult  = 1;
	bool IsPlaceholder         = false;
	bool WasRegisteredByScript = false;
};

inline std::unordered_map<std::string, EffectGroupData> g_dictEffectGroups {
	{ "_group_teleport", { .WeightMult = 2 } },          { "_group_spawngeneric", { .WeightMult = 5 } },
	{ "_group_spawnenemyspecial", { .WeightMult = 5 } }, { "_group_spawnenemy", { .WeightMult = 4 } },
	{ "_group_spawncompanion", { .WeightMult = 5 } },    { "_group_weapons", { .WeightMult = 4 } },
	{ "_group_playerkill", { .WeightMult = 2 } },        { "_group_trafficspawner", { .WeightMult = 3 } },
	{ "_group_trafficcolor", { .WeightMult = 3 } },      { "_group_timechange", { .WeightMult = 2 } },
	{ "_group_weatherchange", { .WeightMult = 2 } },     { "_group_shader", { .WeightMult = 3 } },
};

inline const std::unordered_map<EEffectGroupType, std::string> g_dictEffectTypeToGroup {
	{ EEffectGroupType::Teleport, "_group_teleport" },
	{ EEffectGroupType::SpawnGeneric, "_group_spawngeneric" },
	{ EEffectGroupType::SpawnEnemySpecial, "_group_spawnenemyspecial" },
	{ EEffectGroupType::SpawnEnemy, "_group_spawnenemy" },
	{ EEffectGroupType::SpawnCompanion, "_group_spawncompanion" },
	{ EEffectGroupType::Weapons, "_group_weapons" },
	{ EEffectGroupType::PlayerKill, "_group_playerkill" },
	{ EEffectGroupType::TrafficSpawner, "_group_trafficspawner" },
	{ EEffectGroupType::TrafficColor, "_group_trafficcolor" },
	{ EEffectGroupType::TimeChange, "_group_timechange" },
	{ EEffectGroupType::WeatherChange, "_group_weatherchange" },
	{ EEffectGroupType::Shader, "_group_shader" },
};

inline std::unordered_map<std::string, unsigned short> g_dictEffectGroupMemberCount;
