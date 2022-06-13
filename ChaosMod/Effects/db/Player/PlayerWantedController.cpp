#include <stdafx.h>

#include "Memory/Hooks/ScriptThreadRunHook.h"
#include "Components/EffectDispatcher.h"

static void OnStartFive()
{
	Player player = PLAYER_ID();

	SET_PLAYER_WANTED_LEVEL(player, 5, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
}

// clang-format off
REGISTER_EFFECT(OnStartFive, nullptr, nullptr, EffectInfo
	{
		.Name = "5 Wanted Stars",
		.Id = "player_5stars",
		.IncompatibleWith = { "player_neverwanted" }
	}
);
// clang-format on

static void OnStartPlusTwo()
{
	Ped player = PLAYER_ID();

	SET_PLAYER_WANTED_LEVEL(player, PLAYER::GET_PLAYER_WANTED_LEVEL(player) + 2, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
}

// clang-format off
REGISTER_EFFECT(OnStartPlusTwo, nullptr, nullptr, EffectInfo
	{
		.Name = "+2 Wanted Stars",
		.Id = "player_plus2stars",
		.IncompatibleWith = { "player_neverwanted" }
	}
);
// clang-format on

static void OnTickNeverWanted()
{
	SET_PLAYER_WANTED_LEVEL(PLAYER_ID(), 0, false);
	SET_PLAYER_WANTED_LEVEL_NOW(PLAYER_ID(), true);
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTickNeverWanted, EffectInfo
	{
		.Name = "Never Wanted",
		.Id = "player_neverwanted",
		.IsTimed = true
	}
);
// clang-format on

static void OnStartThree()
{
	Player player = PLAYER_ID();

	SET_PLAYER_WANTED_LEVEL(player, 3, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
}

// clang-format off
REGISTER_EFFECT(OnStartThree, nullptr, nullptr, EffectInfo
	{
		.Name = "3 Wanted Stars",
		.Id = "player_3stars",
		.IncompatibleWith = { "player_neverwanted" }
	}
);
// clang-format on

static void OnStartOne()
{
	Player player = PLAYER_ID();

	SET_PLAYER_WANTED_LEVEL(player, 1, false);
	SET_PLAYER_WANTED_LEVEL_NOW(player, false);
}

// clang-format off
REGISTER_EFFECT(OnStartOne, nullptr, nullptr, EffectInfo
	{
		.Name = "1 Wanted Star",
		.Id = "player_1star",
		.IncompatibleWith = { "player_neverwanted" }
	}
);
// clang-format on

static enum WantedLevelType
{
	SET,
	ADD
};

struct FakeWantedInfo
{
	std::string szName;
	WantedLevelType eWantedType;
	int iStars;
};

std::vector<FakeWantedInfo> m_fakeWantedLevels = 
{ 
	{"5 Wanted Stars", SET, 5}, 
	{"3 Wanted Stars", SET, 3}, 
	{"1 Wanted Star", SET, 1}, 
	{"+2 Wanted Stars", ADD, 2}, 
};

static void OnStartFake()
{
	FakeWantedInfo selectedInfo = m_fakeWantedLevels.at(g_Random.GetRandomInt(0, m_fakeWantedLevels.size()-1));
	WantedLevelType selectedType = selectedInfo.eWantedType;

	Hooks::EnableScriptThreadBlock();

	GetComponent<EffectDispatcher>()->OverrideEffectName("player_fakestars", selectedInfo.szName);

	Player player = PLAYER_ID();
	int lastLevel = GET_PLAYER_WANTED_LEVEL(player);
	int lastLevelf = GET_FAKE_WANTED_LEVEL();

	switch (selectedType)
	{
	case SET:
	{
		SET_FAKE_WANTED_LEVEL(selectedInfo.iStars);
		break;
	}
	case ADD:
	{
		int wl = [&]()
		{

			int l = selectedInfo.iStars + (lastLevelf == 0 ? lastLevel : lastLevelf);
			if (l > 5) l = 5;
			return l;
		}();
		SET_FAKE_WANTED_LEVEL(wl);
		break;
	}
	}

	WAIT(g_Random.GetRandomInt(2500, 6500));

	Hooks::DisableScriptThreadBlock();
	SET_FAKE_WANTED_LEVEL(lastLevelf);
}

// clang-format off
REGISTER_EFFECT(OnStartFake, nullptr, nullptr, EffectInfo
	{
		.Name = "Fake Wanted Level",
		.Id = "player_fakestars",
		.IncompatibleWith = { "player_neverwanted" }
	}
);