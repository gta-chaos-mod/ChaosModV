/*
	Effect by OnlyRealNubs
*/

#include <stdafx.h>

std::vector<std::string> facts = {
	//For a list of things you can put here: https://pastebin.com/nqNYWMSB
	"~INPUT_ATTACK~ Click To Shoot",
	"If You Don't Take Damage You Won't Die",
	"Move with ~INPUT_MOVE_UP~~INPUT_MOVE_LEFT~~INPUT_MOVE_DOWN~~INPUT_MOVE_RIGHT~",
	"If You Die, You're Dead",
	"Bullets Hurt",
	"If You Don't Mission Fail That Would Be Good",
	"If You Are Wanted ~ws~, Cops Will Shoot You",
	"Failing Is Inefficient",
	"Use The Controls To Play",
	"This Is Text",
	"If You Sprint, You Move Faster",
	"~r~Th~o~is ~y~Is ~g~Rain~b~bow ~p~Text",
	"This Game Was Made By Rockstar ~|~",
	"To Fly In A Car, It Has To Have Wings",
	"This Blip ~BLIP_SIMEON_FAMILY~ Is Called \"Simeon Family\" In The Game Code",
	"You Need To Chill",
	"Only $5.99 a Month",
	"This Mod Is Great",
	"This Mod Was Written In C++",
	"~b~I'm Blue",
	"~l~This Text Is Black",
	"You Won A Free iFruit!",
	"Will Sniff (Will Smith)",
	"You Weren't So Bad You Would Be Good",
	"Welcome To The New Story Mode DLC!~n~~n~Oh Wait.. Never Mind.",
	"GTA 6 Confirmed!?!?!?!111??!11",
	".",
};

std::string msg;
int scaleform = 0;

static void _GetMessage(int rng) //Without underscore, it's defined somewhere idek.
{
	msg = facts.at(rng);
}

static void OnStart()
{
	_GetMessage(g_Random.GetRandomInt(0, facts.size()));
	DWORD lastTick = GetTickCount64();
	DWORD time = 3000;
	SET_TIME_SCALE(0.2f);
	while (GetTickCount64() - lastTick < time)
	{
		WAIT(0);
		DISPLAY_HELP_TEXT_THIS_FRAME(msg.c_str(), 0);
	}
	SET_TIME_SCALE(1.f);
}

static RegisterEffect registerEffect(EFFECT_PLAYER_FACTS, OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Useful Fact",
		.Id = "player_facts"
	}
);