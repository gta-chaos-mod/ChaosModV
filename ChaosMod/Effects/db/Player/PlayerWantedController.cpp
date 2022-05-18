#include <stdafx.h>

static void OnStartFive()
{
	Player player = PLAYER_ID();

	SET_PLAYER_WANTED_LEVEL(player, 5, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
}

static RegisterEffect registerEffect(OnStartFive, EffectInfo
	{
		.Name = "5 Wanted Stars",
		.Id = "player_5stars",
		.IncompatibleWith = { "player_neverwanted" }
	}
);
static void OnStartPlusTwo()
{
	Ped player = PLAYER_ID();

	SET_PLAYER_WANTED_LEVEL(player, PLAYER::GET_PLAYER_WANTED_LEVEL(player) + 2, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
}

static RegisterEffect registerEffect2(OnStartPlusTwo, EffectInfo
	{
		.Name = "+2 Wanted Stars",
		.Id = "player_plus2stars",
		.IncompatibleWith = { "player_neverwanted" }
	}
);
static void OnTickNeverWanted()
{
	SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), 0, false);
	SET_PLAYER_WANTED_LEVEL_NOW(PLAYER_ID(), true);
}

static RegisterEffect registerEffect3(nullptr, nullptr, OnTickNeverWanted, EffectInfo
	{
		.Name = "Never Wanted",
		.Id = "player_neverwanted",
		.IsTimed = true
	}
);