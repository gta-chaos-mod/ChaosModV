/*
	Effect by OnlyRealNubs
*/

#include <stdafx.h>

std::vector<std::string> facts = {
	//For a list of things you can put here: https://pastebin.com/nqNYWMSB
	"~INPUT_ATTACK~ Click To Shoot",
	"If You Don't Take Damage You Won't Die",
	"Move with ~INPUTGROUP_VEH_MOVE_ALL~",
	"If You Die, You're Dead",
	"Bullets Hurt",
	"If You Don't Mission Fail That Would Be Good",
	"If You Are Wanted ~ws~, Cops Will Shoot You",
	"Failing Is Inefficient",
	"Use The Controls To Play",
	"This Is Text",
	"If You Sprint, You Move Faster",
	"~r~This ~o~Is ~y~Rainbow ~g~Text; ~b~Wow ~p~So ~HUD_COLOUR_PINK~Cool",
	"This Game Was Made By Rockstar ~∑~",
	"To Fly In A Car, It Has To Have Wings",
	"This Blip ~BLIP_SIMEON_FAMILY~ Is Called \"Simeon Family\" In The Game Code",
	"You Need To Chill",
	"Only $5.99 A Month",
	"This Mod Is Great",
	"This Mod Was Written In C++",
	//"~b~I'm Blue", Seems to crash the game?
	"~l~This Text Is Black",
	"You Won A Free iFruit!",
	"Will Sniff (Will Smith)",
	"If You Weren't So Bad You Would Be Good",
	"Welcome To The New Story Mode DLC!~n~~n~Oh Wait.. Never Mind.",
	"GTA 6 Confirmed!?!?!?!111??!11",
	".",
};



static void _GetMessage(int rng) //Without underscore, it's defined somewhere idek. I think it might be Microsoft
{
	msg = facts.at(rng);
}

static void OnStart()
{
	std::string msg = facst.at(g_Random.GetRandomInt(0, facts.size()));
	DWORD lastTick = GetTickCount64();
	DWORD time = 12000;
	while (GetTickCount64() - lastTick < time)
	{
		WAIT(0);
		HUD::BEGIN_TEXT_COMMAND_DISPLAY_HELP("STRING");
		HUD::ADD_TEXT_COMPONENT_SUBSTRING_PLAYER_NAME(msg.c_str());
		HUD::END_TEXT_COMMAND_DISPLAY_HELP(0, 0, 1, -1);
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_FACTS, OnStart, nullptr, nullptr, EffectInfo
	{
		.Name = "Useful Fact",
		.Id = "player_facts"
	}
);
