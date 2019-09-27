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
	EFFECT_HEAL,
	EFFECT_ARMOR,
	EFFECT_IGNITE,
	EFFECT_ANGRY_JESUS,
	EFFECT_IGNITE_PEDS,
	EFFECT_EXPLODE_VEHS,
	EFFECT_PLAYER_LAUNCH,
	EFFECT_PLAYER_VEH_LAUNCH,
	EFFECT_PLAYER_VEH_LOCK,
	_EFFECT_ENUM_MAX
};

const std::map<EffectType, EffectInfo> Effect =
{
	{EFFECT_KILL, {"Player Suicide"}},
	{EFFECT_PLUS_2_STARS, {"+2 Wanted Stars"}},
	{EFFECT_5_STARS, {"5 Wanted Stars"}},
	{EFFECT_CLEAR_STARS, {"Clear Wanted Stars"}},
	{EFFECT_STRIP_WEAPONS, {"No Weapons For Anyone :("}},
	{EFFECT_GIVE_RPG, {"Give RPG"}},
	{EFFECT_GIVE_MINIGUN, {"Give Minigun"}},
	{EFFECT_HEAL, {"Player Heal"}},
	{EFFECT_ARMOR, {"Player Armor"}},
	{EFFECT_IGNITE, {"Player Ignite"}},
	{EFFECT_ANGRY_JESUS, {"Spawn Griefer Jesus"}},
	{EFFECT_IGNITE_PEDS, {"Ignite All Peds"}},
	{EFFECT_EXPLODE_VEHS, {"Explode All Nearby Vehicles"}},
	{EFFECT_PLAYER_LAUNCH, {"Launch Player In The Air"}},
	{EFFECT_PLAYER_VEH_LAUNCH, {"Launch Player Vehicle In The Air"}},
	{EFFECT_PLAYER_VEH_LOCK, {"Lock Vehicle Player Is In"}}
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
	bool m_activeEffects[_EFFECT_ENUM_MAX] = { false };
};