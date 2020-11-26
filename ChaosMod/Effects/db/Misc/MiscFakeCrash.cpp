#include <stdafx.h>

//Effect by ProfessorBiddle, modified

static void OnStart()
{
	PlaySound(reinterpret_cast<LPCSTR>(SND_ALIAS_SYSTEMHAND), NULL, SND_ALIAS_ID);

	DWORD64 lastTimestamp = GetTickCount64();

	int time = g_random.GetRandomInt(3000, 5000);

	while (lastTimestamp < GetTickCount64() - time)
	{
		
	}
}

static RegisterEffect registerEffect(EFFECT_MISC_CRASH, OnStart);