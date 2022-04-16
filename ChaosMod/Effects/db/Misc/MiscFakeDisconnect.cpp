/*
	Effect by OnlyRealNubs, file created with Last0xygen's tool
*/

#include <stdafx.h>
#include <windows.h>
#include <mmsystem.h>


static void OnStart()
{
	PlaySound(TEXT("C:/Windows/Media/Windows Hardware Remove.wav"), NULL, SND_FILENAME | SND_ASYNC);
}

static RegisterEffect registerEffect(EFFECT_MISC_FAKE_DISCONNECT, OnStart, EffectInfo
	{
		.Name = "Fake Disconnect",
		.Id = "misc_fake_disconnect"
	}
);
