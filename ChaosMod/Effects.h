#pragma once
#include <map>
#include <vector>

enum EffectType : int;

struct EffectInfo
{
public:
	/*EffectInfo(const char* name, bool isTimed, uint duration, std::vector<EffectType> incompatibleList = {})
		: Name(name), IsTimed(isTimed), Duration(duration), IncompatibleWith(incompatibleList) {}*/
	EffectInfo(const char* name, bool isTimed = false, std::vector<EffectType> incompatibleList = {})
		: Name(name), IsTimed(isTimed), Duration(180), IncompatibleWith(incompatibleList)
	{}
	EffectInfo() : Name("???"), IsTimed(false), Duration(0), IncompatibleWith({}) {}

public:
	const char* Name;
	const bool IsTimed;
	const uint Duration;
	const std::vector<EffectType> IncompatibleWith;
};

enum EffectType : int
{
	EFFECT_KILL,
	EFFECT_PLUS_2_STARS,
	EFFECT_5_STARS,
	EFFECT_CLEAR_STARS,
	EFFECT_STRIP_WEAPONS,
	EFFECT_GIVE_RPG,
	EFFECT_GIVE_MINIGUN,
	EFFECT_GIVE_PARACHUTE,
	EFFECT_GIVE_PISTOL,
	EFFECT_GIVE_TAZER,
	EFFECT_GIVE_RAILGUN,
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
	EFFECT_GAMESPEED_X05,
	EFFECT_GAMESPEED_X02,
	EFFECT_PEDS_RIOT,
	EFFECT_RED_VEHS,
	EFFECT_BLUE_VEHS,
	EFFECT_GREEN_VEHS,
	EFFECT_RAINBOW_VEHS,
	EFFECT_FORCED_FP,
	EFFECT_SLIPPERY_VEHS,
	EFFECT_NO_GRAV_VEHS,
	EFFECT_PLAYER_INVINCIBLE,
	EFFECT_2XENGINE_VEHS,
	EFFECT_10XENGINE_VEHS,
	EFFECT_05XENGINE_VEHS,
	EFFECT_SPAWN_TANK,
	EFFECT_SPAWN_ADDER,
	EFFECT_SPAWN_DUMP,
	EFFECT_SPAWN_MONSTER,
	EFFECT_SPAWN_BMX,
	EFFECT_SPAWN_TUG,
	EFFECT_SPAWN_CARGO,
	EFFECT_NEVER_WANTED,
	EFFECT_NO_VEHS,
	EFFECT_NO_PEDS,
	EFFECT_EXPLODE_CUR_VEH,
	EFFECT_PEDS_INVISIBLE,
	EFFECT_VEHS_INVISIBLE,
	EFFECT_NO_RADAR,
	EFFECT_NO_HUD,
	EFFECT_SUPER_RUN,
	EFFECT_SUPER_JUMP,
	_EFFECT_ENUM_MAX
};

const std::map<EffectType, EffectInfo> Effect =
{
	{EFFECT_KILL, {"Player Suicide"}},
	{EFFECT_PLUS_2_STARS, {"+2 Wanted Stars"}},
	{EFFECT_5_STARS, {"5 Wanted Stars"}},
	{EFFECT_CLEAR_STARS, {"Clear Wanted Level"}},
	{EFFECT_STRIP_WEAPONS, {"Remove Weapons From Anyone"}},
	{EFFECT_GIVE_RPG, {"Give RPG"}},
	{EFFECT_GIVE_MINIGUN, {"Give Minigun"}},
	{EFFECT_GIVE_PARACHUTE, {"Give Parachute"}},
	{EFFECT_GIVE_PISTOL, {"Give Pistol"}},
	{EFFECT_GIVE_TAZER, {"Give Tazer"}},
	{EFFECT_GIVE_RAILGUN, {"Give Railgun"}},
	{EFFECT_HEAL, {"Player Heal"}},
	{EFFECT_ARMOR, {"Player Armor"}},
	{EFFECT_IGNITE, {"Ignite Player"}},
	{EFFECT_ANGRY_JESUS, {"Spawn Griefer Jesus"}},
	{EFFECT_IGNITE_PEDS, {"Ignite All Peds"}},
	{EFFECT_EXPLODE_VEHS, {"Explode All Nearby Vehicles"}},
	{EFFECT_PLAYER_LAUNCH, {"Launch Player Up In The Air"}},
	{EFFECT_PLAYER_VEH_LAUNCH, {"Launch Player Vehicle Up In The Air"}},
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
	{EFFECT_GAMESPEED_X02, {"x0.2 Gamespeed", true, { EFFECT_GAMESPEED_X05 }}},
	{EFFECT_GAMESPEED_X05, {"x0.5 Gamespeed", true, { EFFECT_GAMESPEED_X02 }}},
	{EFFECT_PEDS_RIOT, {"Peds Riot", true}},
	{EFFECT_RED_VEHS, {"Red Traffic", true, { EFFECT_BLUE_VEHS, EFFECT_GREEN_VEHS, EFFECT_RAINBOW_VEHS, EFFECT_VEHS_INVISIBLE }}},
	{EFFECT_BLUE_VEHS, {"Blue Traffic", true, { EFFECT_RED_VEHS, EFFECT_GREEN_VEHS, EFFECT_RAINBOW_VEHS, EFFECT_VEHS_INVISIBLE }}},
	{EFFECT_GREEN_VEHS, {"Green Traffic", true, { EFFECT_RED_VEHS, EFFECT_BLUE_VEHS, EFFECT_RAINBOW_VEHS, EFFECT_VEHS_INVISIBLE }}},
	{EFFECT_RAINBOW_VEHS, {"Rainbow Traffic", true, { EFFECT_RED_VEHS, EFFECT_BLUE_VEHS, EFFECT_GREEN_VEHS, EFFECT_VEHS_INVISIBLE }}},
	{EFFECT_FORCED_FP, {"Forced First Person", true}},
	{EFFECT_SLIPPERY_VEHS, {"Slippery Vehicles", true}},
	{EFFECT_NO_GRAV_VEHS, {"Vehicles Have No Gravity", true}},
	{EFFECT_PLAYER_INVINCIBLE, {"Invincibility", true}},
	{EFFECT_2XENGINE_VEHS, {"2x Vehicle Engine Speed", true, { EFFECT_10XENGINE_VEHS, EFFECT_05XENGINE_VEHS }}},
	{EFFECT_10XENGINE_VEHS, {"10x Vehicle Engine Speed", true, { EFFECT_2XENGINE_VEHS, EFFECT_05XENGINE_VEHS }}},
	{EFFECT_05XENGINE_VEHS, {"0.5x Vehicle Engine Speed", true, { EFFECT_2XENGINE_VEHS, EFFECT_10XENGINE_VEHS }}},
	{EFFECT_SPAWN_TANK, {"Spawn Rhino"}},
	{EFFECT_SPAWN_ADDER, {"Spawn Adder"}},
	{EFFECT_SPAWN_DUMP, {"Spawn Dump"}},
	{EFFECT_SPAWN_MONSTER, {"Spawn Monster"}},
	{EFFECT_SPAWN_BMX, {"Spawn BMX"}},
	{EFFECT_SPAWN_TUG, {"Spawn Tug"}},
	{EFFECT_SPAWN_CARGO, {"Spawn Cargo Plane"}},
	{EFFECT_NEVER_WANTED, {"Never Wanted", true}},
	{EFFECT_NO_VEHS, {"No Traffic", true}},
	{EFFECT_NO_PEDS, {"No Pedestrians", true}},
	{EFFECT_EXPLODE_CUR_VEH, {"Explode Current Vehicle"}},
	{EFFECT_PEDS_INVISIBLE, {"Everyone Is A Ghost", true}},
	{EFFECT_VEHS_INVISIBLE, {"Invisible Vehicles", true, { EFFECT_BLUE_VEHS, EFFECT_GREEN_VEHS, EFFECT_RAINBOW_VEHS, EFFECT_RAINBOW_VEHS }}},
	{EFFECT_NO_RADAR, {"No Radar", true, { EFFECT_NO_HUD }}},
	{EFFECT_NO_HUD, {"No HUD", true, { EFFECT_NO_RADAR } }},
	{EFFECT_SUPER_RUN, {"Super Run", true }},
	{EFFECT_SUPER_JUMP, {"Super Jump", true }},
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