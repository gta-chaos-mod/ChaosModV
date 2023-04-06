#pragma once

#include "Natives.h"

namespace Util
{
	inline Vector3 GetGameplayCamOffsetInWorldCoords(const Vector3 &offset)
	{
		// Thanks to scripthookvdotnet!
		auto rot                = GET_GAMEPLAY_CAM_ROT(2);
		auto forward            = rot.GetDirectionForRotation();

		static const double d2R = 0.01745329251994329576923690768489;

		float num1              = cos(rot.y * d2R);
		float x                 = num1 * cos(-rot.z * d2R);
		float y                 = num1 * sin(rot.z * d2R);
		float z                 = sin(-rot.y * d2R);

		auto right              = Vector3::Init(x, y, z);
		auto up                 = Vector3::Cross(right, forward);
		return GET_GAMEPLAY_CAM_COORD() + (right * offset.x) + (forward * offset.y) + (up * offset.z);
	}

	inline Vector3 DegToRadian(const Vector3 &angles)
	{
		return Vector3::Init(angles.x * .0174532925199433F, angles.y * .0174532925199433F,
		                     angles.z * .0174532925199433F);
	}

	inline Vector3 GetCoordsFromGameplayCam(float distance)
	{
		auto rot    = DegToRadian(GET_GAMEPLAY_CAM_ROT(2));
		auto coords = GET_GAMEPLAY_CAM_COORD();

		rot.y       = distance * cos(rot.x);
		coords.x    = coords.x + rot.y * std::sin(rot.z * -1.f);
		coords.y    = coords.y + rot.y * std::cos(rot.z * -1.f);
		coords.z    = coords.z + distance * sin(rot.x);

		return coords;
	}
}