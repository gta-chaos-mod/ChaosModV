/*
	Effect by Gorakh
*/

#include <stdafx.h>
#include "../vendor/scripthookv/inc/enums.h"

static float horizontalOffset;
static float verticalOffset;

static int tickCounter;

static void OnStart()
{
	// Multiply offset by -1, 0, or 1 to either reverse the direction, or disable the offset of that axis entirely
	verticalOffset = g_Random.GetRandomFloat(0.35f, 0.6f) * g_Random.GetRandomInt(-1, 1);

	// A not-very-good fix for no axis getting an offset
	int horizontalMult = g_Random.GetRandomInt(-1, 1);
	if (verticalOffset == 0.f && horizontalMult == 0)
		horizontalMult = 1;

	horizontalOffset = g_Random.GetRandomFloat(0.35f, 0.6f) * horizontalMult;

	tickCounter = 0;
}

static void OffsetControl(int control, float amount)
{
	float value = GET_CONTROL_NORMAL(0, control);
	_SET_CONTROL_NORMAL(0, control, value + amount);
}

static void OnTick()
{
	// Any offset <0.25 is completely ignored, so this seems to be the only way to decrease the "intensity" of the offset
	if (++tickCounter % 5 != 0)
	{
		OffsetControl(eControl::ControlLookLeftRight, horizontalOffset);
		OffsetControl(eControl::ControlLookUpDown, verticalOffset);
	
		OffsetControl(eControl::ControlCursorX, horizontalOffset);
		OffsetControl(eControl::ControlCursorY, verticalOffset);
	
		OffsetControl(eControl::ControlFrontendAxisX, horizontalOffset);
		OffsetControl(eControl::ControlFrontendAxisY, verticalOffset);
	
		OffsetControl(eControl::ControlFrontendRightAxisX, horizontalOffset);
		OffsetControl(eControl::ControlFrontendRightAxisY, verticalOffset);
	
		OffsetControl(eControl::ControlFrontendX, horizontalOffset);
		OffsetControl(eControl::ControlFrontendY, verticalOffset);
	
		OffsetControl(eControl::ControlRadioWheelLeftRight, horizontalOffset);
		OffsetControl(eControl::ControlRadioWheelUpDown, verticalOffset);
	
		OffsetControl(eControl::ControlVehicleGunLeftRight, horizontalOffset);
		OffsetControl(eControl::ControlVehicleGunUpDown, verticalOffset);
	
		OffsetControl(eControl::ControlWeaponWheelLeftRight, horizontalOffset);
		OffsetControl(eControl::ControlWeaponWheelUpDown, verticalOffset);
	
		OffsetControl(eControl::ControlScriptLeftAxisX, horizontalOffset);
		OffsetControl(eControl::ControlScriptLeftAxisX, verticalOffset);
	
		OffsetControl(eControl::ControlScriptRightAxisX, horizontalOffset);
		OffsetControl(eControl::ControlScriptRightAxisX, verticalOffset);
	}

	// Hopefully this isn't going too far :]
	CURSORINFO info{};
	info.cbSize = sizeof(CURSORINFO);
	if (GetCursorInfo(&info))
	{
		SetCursorPos(info.ptScreenPos.x + horizontalOffset * 5, info.ptScreenPos.y + verticalOffset * 5);
	}
}

static RegisterEffect registerEffect(EFFECT_PLAYER_WARPED_CONTROL_STICK, OnStart, nullptr, OnTick, EffectInfo
	{
		.Name = "Controller Experience",
		.Id = "player_warped_control_stick",
		.IsTimed = true,
		.IsShortDuration = true
	}
);