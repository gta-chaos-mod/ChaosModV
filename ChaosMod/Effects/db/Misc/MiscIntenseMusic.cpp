#include <stdafx.h>

static void OnStart()
{
	TRIGGER_MUSIC_EVENT("AW_LOBBY_MUSIC_START");
}

static void OnStop()
{
	TRIGGER_MUSIC_EVENT("MP_MC_CMH_IAA_FINALE_START");
}

static RegisterEffect registerEffect(OnStart, OnStop, EffectInfo
	{
		.Name = "Play Arena Wars Theme",
		.Id = "player_arenawarstheme",
		.IsTimed = true
	}
);