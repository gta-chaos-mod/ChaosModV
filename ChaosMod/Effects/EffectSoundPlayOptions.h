#pragma once

#include <scripthookv/inc/types.h>

enum class EffectSoundPlayType
{
	FollowPlayer,
	FollowEntity,
	AtCoords
};
struct EffectSoundPlayOptions
{
	EffectSoundPlayType PlayType = EffectSoundPlayType::FollowPlayer;
	union
	{
		Entity Entity;
		Vector3 Coords;
	};
};