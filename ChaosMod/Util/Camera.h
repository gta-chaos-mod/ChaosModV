#pragma once

#include "nativesNoNamespaces.h"

inline Vector3 GetGameplayCamOffsetInWorldCoords(Vector3 offset)
{
	// Thanks to scripthookvdotnet!
	Vector3 rot = GET_GAMEPLAY_CAM_ROT(2);
	Vector3 Forward = rot.GetDirectionForRotation();
	const double D2R = 0.01745329251994329576923690768489;
	float num1 = cos(rot.y * D2R);
	float x = num1 * cos(-rot.z * D2R);
	float y = num1 * sin(rot.z * D2R);
	float z = sin(-rot.y * D2R);
	Vector3 Right = Vector3::Init(x, y, z);
	Vector3 Up = Vector3::Cross(Right, Forward);
	return GET_GAMEPLAY_CAM_COORD() + (Right * offset.x) + (Forward * offset.y) + (Up * offset.z);
}
