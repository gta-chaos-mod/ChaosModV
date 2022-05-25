/*
    Effect by DrUnderscore (James), modified
*/

#include <stdafx.h>

static const char *TV_PLAYLISTS[] = {
	"PL_WEB_KFLF",    // Kung Fu Rainbow Lazerforce
	"PL_WEB_RANGERS", // Republican Space Rangers
	"PL_WEB_PRB2",    // Princess Robot Bubblegum
	"PL_WEB_FOS",     // Fame or Shame
	"PL_WEB_CAS",     // Diamond Casino DLC intro
	"PL_WEB_FOS",
	"PL_WEB_HOWITZER", // Howitzer Documentary
	"PL_WEB_KFLF",
	"PL_WEB_PRB2",
	"PL_WEB_RS",
	"PL_STD_CNT",
	"PL_STD_WZL",
	"PL_LO_CNT",
	"PL_LO_WZL",
	"PL_SP_WORKOUT",
	"PL_SP_INV",
	"PL_SP_INV_EXP",
	"PL_LO_RS",
	"PL_LO_RS_CUTSCENE",
	"PL_SP_PLSH1_INTRO",
	"PL_LES1_FAME_OR_SHAME",
	"PL_STD_WZL_FOS_EP2",
	"PL_MP_WEAZEL",
	"PL_MP_CCTV",
	"PL_CINEMA_CARTOON",
	"PL_CINEMA_ARTHOUSE",
	"PL_CINEMA_ACTION",
	"PL_CINEMA_MULTIPLAYER",
	"PL_CINEMA_MULTIPLAYER_NO_MELTDOWN",
};

static float ms_fPosX = 0.f;
static float ms_fPosY = 0.f;

static void OnStart()
{
	const char *szPlaylist = TV_PLAYLISTS[g_Random.GetRandomInt(0, sizeof(TV_PLAYLISTS) / sizeof(TV_PLAYLISTS[0]) - 1)];

	GRAPHICS::SET_TV_CHANNEL_PLAYLIST_AT_HOUR(0, szPlaylist, g_Random.GetRandomInt(0, 23));
	GRAPHICS::SET_TV_AUDIO_FRONTEND(true);
	GRAPHICS::SET_TV_VOLUME(1.0f); // 0.0 is actually the highest the player can normally tune to.
	GRAPHICS::ATTACH_TV_AUDIO_TO_ENTITY(PLAYER_PED_ID());
	GRAPHICS::SET_TV_CHANNEL(0);
	GRAPHICS::ENABLE_MOVIE_SUBTITLES(true);

	ms_fPosX = g_Random.GetRandomFloat(0.3f, 0.7f);
	ms_fPosY = g_Random.GetRandomFloat(0.3f, 0.7f);
}

static void OnStop()
{
	GRAPHICS::SET_TV_CHANNEL(-1);
	GRAPHICS::SET_TV_CHANNEL_PLAYLIST(0, nullptr, false);
	GRAPHICS::ENABLE_MOVIE_SUBTITLES(false);
}

static void OnTick()
{
	GRAPHICS::SET_SCRIPT_GFX_DRAW_ORDER(4);
	GRAPHICS::SET_SCRIPT_GFX_DRAW_BEHIND_PAUSEMENU(1);
	GRAPHICS::DRAW_TV_CHANNEL(ms_fPosX, ms_fPosY, 0.3f, 0.3f, 0.0f, 255, 255, 255, 255);
}

// clang-format off
REGISTER_EFFECT(OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "On-Demand TV",
		.Id = "player_on_demand_cartoon",
		.IsTimed = true
	}
);