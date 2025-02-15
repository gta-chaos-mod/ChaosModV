#pragma once

#include <scripthookv/inc/types.h>

enum class EffectSoundPlayType
{
	FollowPlayer,
	FollowEntity,
	AtCoords
};

enum EffectSoundPlayFlags
{
	EffectSoundPlayFlags_Looping               = (1 << 0),
	EffectSoundPlayFlags_DontStopOnEntityDeath = (1 << 1)
};

struct EffectSoundPlayOptions
{
	EffectSoundPlayType PlayType = EffectSoundPlayType::FollowPlayer;
	int PlayFlags;
	union
	{
		Entity Entity;
		Vector3 Coords;
	};
};