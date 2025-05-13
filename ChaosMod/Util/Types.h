#pragma once

#include "Natives.h"

template <typename T> union ChaosVector2
{
	T Data[2];
	struct
	{
		T X, Y;
	};

	constexpr ChaosVector2(T x, T y) : X(x), Y(y)
	{
	}

	constexpr ChaosVector2() : X(), Y()
	{
	}
};

template <typename T> union ChaosVector3
{
	T Data[3];
	struct
	{
		T X, Y, Z;
	};

	constexpr ChaosVector3(T x, T y, T z) noexcept : X(x), Y(y), Z(z)
	{
	}

	constexpr ChaosVector3() noexcept : X(), Y(), Z()
	{
	}

	explicit ChaosVector3(const Vector3 &vec) : X(vec.x), Y(vec.y), Z(vec.z)
	{
	}
};

typedef ChaosVector2<float> fChaosVector2;
typedef ChaosVector3<float> fChaosVector3;

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