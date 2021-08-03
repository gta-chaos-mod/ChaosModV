#include <stdafx.h>

//Effect by ProfessorBiddle

static void OnStop()
{
	REMOVE_ANIM_DICT("friends@frj@ig_1");
}

static void OnTick()
{
	REQUEST_ANIM_DICT("friends@frj@ig_1");
	Ped playerPed = PLAYER_PED_ID();

	static DWORD64 lastTick = 0;
	DWORD64 curTick = GET_GAME_TIMER();

	if (lastTick < curTick - 5000)
	{
		lastTick = curTick;

		for (Ped ped : GetAllPeds())
		{
			if (!IS_ENTITY_PLAYING_ANIM(ped, "friends@frj@ig_1", "wave_a", 3) && !IS_PED_A_PLAYER(ped) && IS_PED_HUMAN(ped) && !IS_ENTITY_A_MISSION_ENTITY(ped))
			{
				TASK_TURN_PED_TO_FACE_ENTITY(ped, playerPed, 1);
				TASK_PLAY_ANIM(ped, "friends@frj@ig_1", "wave_a", 4.0f, -4.0f, -1.f, 1, 0.f, false, false, false);
				constexpr const char* speechesFriendly[3]{ "GENERIC_HI" };

				int choice = g_Random.GetRandomInt(0, 2);
				_PLAY_AMBIENT_SPEECH1(ped, speechesFriendly[choice], "SPEECH_PARAMS_FORCE_SHOUTED", 1);
			}
		}
	}


}

static RegisterEffect registerEffect(EFFECT_PEDS_WAVE_HELLO, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Peds Wave Hello",
		.Id = "peds_wave_hello",
		.IsTimed = true
	}
);