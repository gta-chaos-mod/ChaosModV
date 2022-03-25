/*
	Effect by OnlyRealNubs
*/

#include <stdafx.h>

static void OnStart()
{
	DESTROY_MOBILE_PHONE();
	CELL_CAM_ACTIVATE(1,0);
}


// Any of these functions can be omitted and either replaced with a `nullptr` or completely left out (default parameter values) in the `RegisterEffect` declaration
static RegisterEffect registerEffect(EEffectType::EFFECT_MISC_SELFIE_TIME, OnStart, EffectInfo
	{
		// These are always required, you may have to add more designators depending on your effect
		.Name = "Selfie Time",
		.Id = "misc_selfie_time"
	}
);