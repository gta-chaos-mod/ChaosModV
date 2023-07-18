#pragma once

#include "Natives.h"

struct ChaosVector3
{
	float x, y, z;
	explicit ChaosVector3(const Vector3 &vec) : x(vec.x), y(vec.y), z(vec.z)
	{
	}
};

struct ChaosVector2
{
	float x, y;
};

inline Vector3 GetCoordAround(Entity entity, float angle, float radius, float zOffset, bool relative)
{
	if (relative)
	{
		Vector3 offset = Vector3::Init(-radius * SIN(angle + 90), radius * SIN(angle), zOffset);

		return GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(entity, offset.x, offset.y, offset.z);
	}
	else
	{
		Vector3 entityPosition = GET_ENTITY_COORDS(entity, false);
		return Vector3::Init(entityPosition.x - radius * SIN(angle + 90), entityPosition.y + radius * SIN(angle),
		                     entityPosition.z + zOffset);
	}
}