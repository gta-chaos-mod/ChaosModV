/*
	Effect by OnlyRealNubs, file created with Last0xygen's tool
*/

#include <stdafx.h>
#include <windows.h>
#include <mmsystem.h>

static void OnStart()
{
	static int disableTime = g_Random.GetRandomInt(900, 3000);
	Player player = PLAYER_ID();
	PlaySound(TEXT("C:/Windows/Media/Windows Hardware Remove.wav"), NULL, SND_FILENAME | SND_ASYNC);
	SET_PLAYER_CONTROL(player, 0, 1 << 8);
	WAIT(disableTime);
	SET_PLAYER_CONTROL(player, 1, 1 << 8);
}

static RegisterEffect registerEffect(EFFECT_MISC_FAKE_DISCONNECT, OnStart, EffectInfo
	{
		.Name = "Fake Disconnect",
		.Id = "misc_fake_disconnect"
	}
);
