#include "stdafx.h"

std::string name = "rockstar_logos";

static void OnStart()
{
	int bik = _SET_BINK_MOVIE_REQUESTED(name.c_str());
	_SET_BINK_MOVIE_PROGRESS(bik, 0.f);
	int w, h;
	GET_SCREEN_RESOLUTION(&w, &h);
	_PLAY_BINK_MOVIE(bik);
	while (_GET_BINK_MOVIE_PROGRESS(bik) < 100.f)
	{	
		_DRAW_BINK_MOVIE(bik, 0.f, 0.f, 0.f, w, h, 255, 255, 255, 255);
		WAIT(0);
	}
	_STOP_BINK_MOVIE(bik);
	_RELEASE_BINK_MOVIE(bik);
}

static RegisterEffect reg(EFFECT_MISC_INTRO, OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Play Intro",
		.Id = "misc_intro"
	}
);