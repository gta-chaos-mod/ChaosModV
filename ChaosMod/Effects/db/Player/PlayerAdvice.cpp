/*
	Effect by OnlyRealNubs
*/

#include <stdafx.h>

std::vector<std::string> facts = {
	"Click To Shoot",
	"If You Don't Take Damage You Won't Die",
	"Move with WASD",
	"If You Die, You're Dead",
	"Bullets Hurt",
	"If You Don't Mission Fail That Would Be Good",
	"If You Are Wanted, Cops Will Shoot You",
	"Failing Is Inefficient",
	"Use The Controls To Play",
	"This Is Text",
	"If You Sprint, You Move Faster"
};

std::string msg;
int scaleform = 0;

static void RequestShit()
{
	scaleform = REQUEST_SCALEFORM_MOVIE("MP_BIG_MESSAGE_FREEMODE");
	while (!HAS_SCALEFORM_MOVIE_LOADED(scaleform))
	{
		WAIT(0);
	}
	BEGIN_SCALEFORM_MOVIE_METHOD(scaleform, "SHOW_SHARD_RANKUP_MP_MESSAGE");
	SCALEFORM_MOVIE_METHOD_ADD_PARAM_PLAYER_NAME_STRING(msg.c_str());
	END_SCALEFORM_MOVIE_METHOD();
}

static void _GetMessage(int rng) //Without underscore, it's defined somewhere idek.
{
	msg = facts.at(rng);
}

static void OnStart()
{
	_GetMessage(g_Random.GetRandomInt(0, facts.size()));
	RequestShit();
	DWORD lastTick = GetTickCount64();
	DWORD time = 3000;
	SET_TIME_SCALE(0.2f);
	while (GetTickCount64() - lastTick < time)
	{
		WAIT(0);
		DRAW_SCALEFORM_MOVIE_FULLSCREEN(scaleform, 255, 255, 255, 255, 0);
	}
	SET_TIME_SCALE(1.f);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_FACTS, OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Useful Fact",
		.Id = "player_facts"
	}
);