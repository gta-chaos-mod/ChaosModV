/*
	Effect by Gorakh
*/

#include <stdafx.h>
#include "../vendor/scripthookv/inc/enums.h"
#include "Util/Window.h"

static float horizontalOffset;
static float verticalOffset;

static int tickCounter;

static void OnStart()
{
	verticalOffset = g_Random.GetRandomFloat(0.2f, 0.8f) * (g_Random.GetRandomInt(0, 1) ? 1 : -1);

	// If both verticalOffset and horizontalOffset are <0.25, the controls won't get offset at all. horizonalOffset gets a higher lower bound in order to prevent the case where both are <0.25, making the effect do almost nothing
	float horizontalMin = abs(verticalOffset) <= 0.25f ? 0.3f : 0.2f;
	horizontalOffset = g_Random.GetRandomFloat(horizontalMin, 0.8f) * (g_Random.GetRandomInt(0, 1) ? 1 : -1);

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
		OffsetControl(eControl::ControlScriptLeftAxisY, verticalOffset);

		OffsetControl(eControl::ControlScriptRightAxisX, horizontalOffset);
		OffsetControl(eControl::ControlScriptRightAxisY, verticalOffset);
	}

	HWND gameWindow = Util::GetGameWindowHandle();
	if (IsWindowEnabled(gameWindow) && IsWindowVisible(gameWindow))
	{
		CURSORINFO info{};
		info.cbSize = sizeof(CURSORINFO);
		if (GetCursorInfo(&info))
		{
			// If mouse cursor is hovering over the game window, not the best solution, but this is the best I could get working
			if (WindowFromPoint(info.ptScreenPos) == gameWindow)
			{
				// Hopefully this isn't going too far :]
				SetCursorPos(info.ptScreenPos.x + horizontalOffset * 5, info.ptScreenPos.y + verticalOffset * 5);
			}
		}
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