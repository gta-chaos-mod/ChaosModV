#pragma once

#include <stdafx.h>

#include <xinput.h>

namespace XInput
{
	//Doesn't include the new Xbox Series X|S rumbles
	inline void SetControllerRumble(DWORD controllerId, int leftMotorSpeed, int rightMotorSpeed)
	{
		XINPUT_VIBRATION vibration;
		ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

		vibration.wLeftMotorSpeed = leftMotorSpeed;
		vibration.wRightMotorSpeed = rightMotorSpeed;

		XInputSetState(controllerId, &vibration);
	}
	
	//Make sure you call stop, or it will rumble until the application closes.
	inline void SetAllControllersRumble(int leftMotorSpeed, int rightMotorSpeed)
	{
		for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
		{
			SetControllerRumble(i, leftMotorSpeed, rightMotorSpeed);
		}
	}

	inline void StopControllerRumble(DWORD controllerId)
	{
		XINPUT_VIBRATION vibration;
		ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

		vibration.wLeftMotorSpeed = 0;
		vibration.wRightMotorSpeed = 0;

		XInputSetState(controllerId, &vibration);
	}

	inline void StopAllControllersRumble()
	{
		for (DWORD i = 0; i < XUSER_MAX_COUNT; i++)
		{
			StopControllerRumble(i);
		}
	}
}
