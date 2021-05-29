#pragma once

#include "Natives.h"

inline Vector3 GetGameplayCamOffsetInWorldCoords(Vector3 offset)
{
	// Thanks to scripthookvdotnet!
	Vector3 rot = GET_GAMEPLAY_CAM_ROT(2);
	Vector3 Forward = rot.GetDirectionForRotation();

	const static double dD2R = 0.01745329251994329576923690768489;

	float fNum1 = cos(rot.y * dD2R);
	float fX = fNum1 * cos(-rot.z * dD2R);
	float fY = fNum1 * sin(rot.z * dD2R);
	float fZ = sin(-rot.y * dD2R);

	Vector3 Right = Vector3::Init(fX, fY, fZ);
	Vector3 Up = Vector3::Cross(Right, Forward);

	return GET_GAMEPLAY_CAM_COORD() + (Right * offset.x) + (Forward * offset.y) + (Up * offset.z);
}
