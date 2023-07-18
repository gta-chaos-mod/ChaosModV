#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

enum class EffectGroupType
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
	Shader,            // Shader effects
	WantedLevel        // Effects that modify or set the players wanted level
};

struct EffectGroupData
{
	std::uint16_t MemberCount  = 0;
	std::uint16_t WeightMult   = 1;
	bool IsPlaceholder         = false;
	bool WasRegisteredByScript = false;
};

inline std::unordered_map<std::string, EffectGroupData> g_EffectGroups {
	{ "_group_teleport", { .WeightMult = 2 } },          { "_group_spawngeneric", { .WeightMult = 5 } },
	{ "_group_spawnenemyspecial", { .WeightMult = 5 } }, { "_group_spawnenemy", { .WeightMult = 4 } },
	{ "_group_spawncompanion", { .WeightMult = 5 } },    { "_group_weapons", { .WeightMult = 4 } },
	{ "_group_playerkill", { .WeightMult = 2 } },        { "_group_trafficspawner", { .WeightMult = 3 } },
	{ "_group_trafficcolor", { .WeightMult = 3 } },      { "_group_timechange", { .WeightMult = 2 } },
	{ "_group_weatherchange", { .WeightMult = 2 } },     { "_group_shader", { .WeightMult = 3 } },
	{ "_group_wantedlevel", { .WeightMult = 2 } },
};

inline const std::unordered_map<EffectGroupType, std::string> g_EffectTypeToGroup {
	{ EffectGroupType::Teleport, "_group_teleport" },
	{ EffectGroupType::SpawnGeneric, "_group_spawngeneric" },
	{ EffectGroupType::SpawnEnemySpecial, "_group_spawnenemyspecial" },
	{ EffectGroupType::SpawnEnemy, "_group_spawnenemy" },
	{ EffectGroupType::SpawnCompanion, "_group_spawncompanion" },
	{ EffectGroupType::Weapons, "_group_weapons" },
	{ EffectGroupType::PlayerKill, "_group_playerkill" },
	{ EffectGroupType::TrafficSpawner, "_group_trafficspawner" },
	{ EffectGroupType::TrafficColor, "_group_trafficcolor" },
	{ EffectGroupType::TimeChange, "_group_timechange" },
	{ EffectGroupType::WeatherChange, "_group_weatherchange" },
	{ EffectGroupType::Shader, "_group_shader" },
	{ EffectGroupType::WantedLevel, "_group_wantedlevel" },
};