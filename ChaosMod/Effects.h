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
	EFFECT_NO_STARS,
	EFFECT_STRIP_WEAPONS,
	EFFECT_GIVE_RPG,
	EFFECT_GIVE_MINIGUN,
	EFFECT_HEAL,
	EFFECT_ARMOR,
	EFFECT_IGNITE,
	EFFECT_ANGRY_JESUS,
	_EFFECT_ENUM_MAX
};

const std::map<EffectType, EffectInfo> Effect =
{
	{EFFECT_KILL, {"Player Suicide"}},
	{EFFECT_PLUS_2_STARS, {"+2 Wanted Stars"}},
	{EFFECT_5_STARS, {"5 Wanted Stars"}},
	{EFFECT_NO_STARS, {"No Wanted Stars"}},
	{EFFECT_STRIP_WEAPONS, {"No weapons :("}},
	{EFFECT_GIVE_RPG, {"Free RPG"}},
	{EFFECT_GIVE_MINIGUN, {"Free Minigun!"}},
	{EFFECT_HEAL, {"Free Heal!"}},
	{EFFECT_ARMOR, {"Player Armor"}},
	{EFFECT_IGNITE, {"BURN!"}},
	{EFFECT_ANGRY_JESUS, {"Spawn Griefer Jesus"}}
};

class Effects
{
public:
	Effects() {}

public:
	void StartEffect(EffectType effectType);
	void StopEffect(EffectType effectType);
};