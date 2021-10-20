#include <stdafx.h>


static std::string options[15] =
{ "Nothing",
"All Peds Are Peds",
"Teleport To Current Location",
"Expanded & Enhanced",
"Spawn Air",
"Destroy All Destroyed Vehicles",
"Kill All Dead Peds",
"+0 Wanted Stars",
"Jesus Take No Wheel",
"Set Time To Current Time",
"Set Player Into Current Vehicle",
"1x Vehicle Engine Speed",
"You Aren't Famous",
"GTA 5",
"Crimes Are Illegal" }; 


static void OnStart()
{

	int start = GET_GAME_TIMER();
	std::string override = options[g_Random.GetRandomInt(0, options->size() - 1)];
	while (start + 25000 > GET_GAME_TIMER())
	{
		WAIT(0);
		g_pEffectDispatcher->OverrideEffectName(EFFECT_NOTHING, override);


		
		
	}
}
static RegisterEffect registerEffect(EFFECT_NOTHING, OnStart, EffectInfo
	{
		.Name = "Nothing",
		.Id = "nothing"
	}
);