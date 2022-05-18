#include <stdafx.h>

static void OnStartSix()
{
	Player player = PLAYER_ID();

	SET_FAKE_WANTED_LEVEL(6);	
	
	SET_PLAYER_WANTED_LEVEL(player, 5, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
	SET_WANTED_LEVEL_MULTIPLIER(5.f);
}

static void OnStopSix()
{
	Player player = PLAYER_ID();

	SET_FAKE_WANTED_LEVEL(0);
	SET_PLAYER_WANTED_LEVEL(player, 0, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
}

static RegisterEffect registerEffect4(EFFECT_6_STARS, OnStartSix, OnStopSix, nullptr, EffectInfo
	{
		.Name = "6 Wanted Stars",
		.Id = "player_6stars",
		.IsTimed = true,
		.IncompatibleWith = {EFFECT_NEVER_WANTED}
	}
);

static void OnStartFive()
{
	Player player = PLAYER_ID();

	SET_PLAYER_WANTED_LEVEL(player, 5, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
}

static RegisterEffect registerEffect(EFFECT_5_STARS, OnStartFive, EffectInfo
	{
		.Name = "5 Wanted Stars",
		.Id = "player_5stars",
		.IncompatibleWith = {EFFECT_NEVER_WANTED}
	}
);

static void OnStartPlusTwo()
{
	Ped player = PLAYER_ID();

	SET_PLAYER_WANTED_LEVEL(player, PLAYER::GET_PLAYER_WANTED_LEVEL(player) + 2, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
}

static RegisterEffect registerEffect2(EFFECT_PLUS_2_STARS, OnStartPlusTwo, EffectInfo
	{
		.Name = "+2 Wanted Stars",
		.Id = "player_plus2stars",
		.IncompatibleWith = {EFFECT_NEVER_WANTED}
	}
);

static void OnTickNeverWanted()
{
	SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), 0, false);
	SET_PLAYER_WANTED_LEVEL_NOW(PLAYER_ID(), true);
}

static RegisterEffect registerEffect3(EFFECT_NEVER_WANTED, nullptr, nullptr, OnTickNeverWanted, EffectInfo
	{
		.Name = "Never Wanted",
		.Id = "player_neverwanted",
		.IsTimed = true
	}
);
