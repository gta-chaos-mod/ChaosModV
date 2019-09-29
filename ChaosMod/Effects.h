#pragma once
#include <map>

struct EffectInfo
{
public:
	EffectInfo(const char* name, bool isTimed = false, uint duration = 180) : Name(name), IsTimed(isTimed), Duration(duration) {}
	EffectInfo() : Name("???"), IsTimed(false), Duration(0) {}

public:
	const char* Name;
	const bool IsTimed;
	const uint Duration;
};

enum EffectType
{
	EFFECT_KILL,
	EFFECT_PLUS_2_STARS,
	EFFECT_5_STARS,
	EFFECT_CLEAR_STARS,
	EFFECT_STRIP_WEAPONS,
	EFFECT_GIVE_RPG,
	EFFECT_GIVE_MINIGUN,
	EFFECT_GIVE_PARACHUTE,
	EFFECT_HEAL,
	EFFECT_ARMOR,
	EFFECT_IGNITE,
	EFFECT_ANGRY_JESUS,
	EFFECT_IGNITE_PEDS,
	EFFECT_EXPLODE_VEHS,
	EFFECT_PLAYER_LAUNCH,
	EFFECT_PLAYER_VEH_LAUNCH,
	EFFECT_PLAYER_VEH_LOCK,
	EFFECT_NOTHING,
	EFFECT_KILL_ENGINE,
	EFFECT_TIME_MORNING,
	EFFECT_TIME_DAY,
	EFFECT_TIME_EVENING,
	EFFECT_TIME_NIGHT,
	EFFECT_WEATHER_SUNNY,
	EFFECT_WEATHER_EXTRASUNNY,
	EFFECT_WEATHER_RAINY,
	EFFECT_WEATHER_THUNDER,
	EFFECT_WEATHER_FOGGY,
	EFFECT_WEATHER_NEUTRAL,
	EFFECT_WEATHER_XMAS,
	EFFECT_TP_LSAIRPORT,
	EFFECT_TP_MAZETOWER,
	EFFECT_TP_FORTZANCUDO,
	EFFECT_TP_MOUNTCHILLIAD,
	EFFECT_NO_PHONE,
	EFFECT_SET_INTO_CLOSEST_VEH,
	EFFECT_EXIT_CURRENT_VEH,
	EFFECT_GAMESPEED_X06,
	EFFECT_GAMESPEED_X02,
	_EFFECT_ENUM_MAX
};

const std::map<EffectType, EffectInfo> Effect =
{
	{EFFECT_KILL, {"Player Suicide"}},
	{EFFECT_PLUS_2_STARS, {"+2 Wanted Stars"}},
	{EFFECT_5_STARS, {"5 Wanted Stars"}},
	{EFFECT_CLEAR_STARS, {"Clear Wanted Level"}},
	{EFFECT_STRIP_WEAPONS, {"No Weapons For Anyone :("}},
	{EFFECT_GIVE_RPG, {"Give RPG"}},
	{EFFECT_GIVE_MINIGUN, {"Give Minigun"}},
	{EFFECT_GIVE_PARACHUTE, {"Give Parachute"}},
	{EFFECT_HEAL, {"Player Heal"}},
	{EFFECT_ARMOR, {"Player Armor"}},
	{EFFECT_IGNITE, {"Ignite Player"}},
	{EFFECT_ANGRY_JESUS, {"Spawn Griefer Jesus"}},
	{EFFECT_IGNITE_PEDS, {"Ignite All Peds"}},
	{EFFECT_EXPLODE_VEHS, {"Explode All Nearby Vehicles"}},
	{EFFECT_PLAYER_LAUNCH, {"Launch Player In The Air"}},
	{EFFECT_PLAYER_VEH_LAUNCH, {"Launch Player Vehicle In The Air"}},
	{EFFECT_PLAYER_VEH_LOCK, {"Lock Vehicle Player Is In"}},
	{EFFECT_NOTHING, {"Nothing"}},
	{EFFECT_KILL_ENGINE, {"Kill Engine Of Player Vehicle"}},
	{EFFECT_TIME_MORNING, {"Set Time To Morning"}},
	{EFFECT_TIME_DAY, {"Set Time To Daytime"}},
	{EFFECT_TIME_EVENING, {"Set Time To Evening"}},
	{EFFECT_TIME_NIGHT, {"Set Time To Night"}},
	{EFFECT_WEATHER_SUNNY, {"Sunny Weather"}},
	{EFFECT_WEATHER_EXTRASUNNY, {"Extra Sunny Weather"}},
	{EFFECT_WEATHER_RAINY, {"Rainy Weather"}},
	{EFFECT_WEATHER_THUNDER, {"Stormy Weather"}},
	{EFFECT_WEATHER_FOGGY, {"Foggy Weather"}},
	{EFFECT_WEATHER_XMAS, {"Snowy Weather"}},
	{EFFECT_TP_LSAIRPORT, {"Teleport To LS Airport"}},
	{EFFECT_TP_MAZETOWER, {"Teleport To Maze Bank Tower"}},
	{EFFECT_TP_FORTZANCUDO, {"Teleport To Fort Zancudo"}},
	{EFFECT_TP_MOUNTCHILLIAD, {"Teleport To Mount Chilliad"}},
	{EFFECT_NO_PHONE, {"No Phone", true}},
	{EFFECT_SET_INTO_CLOSEST_VEH, {"Set Player Into Closest Vehicle"}},
	{EFFECT_EXIT_CURRENT_VEH, {"Exit Current Vehicle"}},
	{EFFECT_GAMESPEED_X02, {"x0.2 Gamespeed", true}},
	{EFFECT_GAMESPEED_X06, {"x0.6 Gamespeed", true}},
};

class Effects
{
public:
	Effects() {}

public:
	void StartEffect(EffectType effectType);
	void StopEffect(EffectType effectType);
	void UpdateEffects();

private:
	bool m_effectActive[_EFFECT_ENUM_MAX] = { false };
};