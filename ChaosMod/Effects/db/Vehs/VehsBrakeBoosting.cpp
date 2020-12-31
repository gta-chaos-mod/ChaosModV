#include <stdafx.h>

static void OnTick()
{

	for (auto veh : GetAllVehs())
	{
		bool isBraking = false;
		if (IS_CONTROL_PRESSED(0, 72) || IS_CONTROL_PRESSED(0, 76) || IS_CONTROL_PRESSED(0, 88) || IS_CONTROL_PRESSED(0, 138) || IS_CONTROL_PRESSED(0, 139) || IS_CONTROL_PRESSED(0, 152) || IS_CONTROL_PRESSED(0, 153))
		{
			isBraking = true;
		}
		if (isBraking)
		{
			APPLY_FORCE_TO_ENTITY(veh, 0, .0f, 50.f, .0f, .0f, .0f, .0f, 0, true, true, true, false, true);
		}
	}
}

static RegisterEffect registerEffect(EFFECT_VEH_BRAKEBOOST, nullptr, nullptr, OnTick);