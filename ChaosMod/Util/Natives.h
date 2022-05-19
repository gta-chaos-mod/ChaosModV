#pragma once

#include "Memory/Physics.h"

#include <scripthookv/inc/natives.h>

#define _NODISCARD [[nodiscard]]

using namespace PLAYER;
using namespace ENTITY;
using namespace PED;
using namespace VEHICLE;
using namespace OBJECT;
using namespace BRAIN;
using namespace TASK;
using namespace MISC;
using namespace AUDIO;
using namespace CUTSCENE;
using namespace INTERIOR;
using namespace CAM;
using namespace WEAPON;
using namespace ITEMSET;
using namespace STREAMING;
using namespace SCRIPT;
using namespace HUD;
using namespace GRAPHICS;
using namespace STATS;
using namespace BRAIN;
using namespace MOBILE;
using namespace APP;
using namespace CLOCK;
using namespace PATHFIND;
using namespace PAD;
using namespace DATAFILE;
using namespace FIRE;
using namespace EVENT;
using namespace ZONE;
using namespace PHYSICS;
using namespace WATER;
using namespace SHAPETEST;
using namespace NETWORK;
using namespace MONEY;
using namespace DLC;
using namespace SYSTEM;
using namespace DECORATOR;
using namespace SOCIALCLUB;

_NODISCARD constexpr inline int _strlen(const char *str)
{
	return *str ? 1 + _strlen(str + 1) : 0;
}

_NODISCARD constexpr inline char __tolower(const char c)
{
	return c >= 'A' && c <= 'Z' ? c + 'a' - 'A' : c;
}

// Thanks to menyoo!
_NODISCARD constexpr inline Hash GET_HASH_KEY(const char *str)
{
	int length = _strlen(str);

	DWORD hash, i;
	for (hash = i = 0; i < length; ++i)
	{
		hash += __tolower(str[i]);
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

inline void SET_ENTITY_AS_NO_LONGER_NEEDED(Entity *entity)
{
	SET_ENTITY_AS_MISSION_ENTITY(*entity, true, true);
	invoke<Void>(0xB736A491E64A32CF, entity); // orig native
}

inline void SET_OBJECT_AS_NO_LONGER_NEEDED(Object *prop)
{
	SET_ENTITY_AS_NO_LONGER_NEEDED(prop);
}

inline void SET_PED_AS_NO_LONGER_NEEDED(Ped *ped)
{
	SET_ENTITY_AS_NO_LONGER_NEEDED(ped);
}

inline void SET_VEHICLE_AS_NO_LONGER_NEEDED(Vehicle *veh)
{
	SET_ENTITY_AS_NO_LONGER_NEEDED(veh);
}

NATIVE_DECL void APPLY_FORCE_TO_ENTITY(Entity entity, int forceFlags, float x, float y, float z, float offX, float offY,
                                       float offZ, int boneIndex, BOOL isDirectionRel, BOOL ignoreUpVec,
                                       BOOL isForceRel, BOOL p12, BOOL p13)
{
	Memory::ApplyForceToEntity(entity, forceFlags, x, y, z, offX, offY, offZ, boneIndex, isDirectionRel, ignoreUpVec,
	                           isForceRel, p12, p13);
}

NATIVE_DECL void APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS(Entity entity, int forceType, float x, float y, float z, BOOL p5,
                                                      BOOL isDirectionRel, BOOL isForceRel, BOOL p8)
{
	Memory::ApplyForceToEntityCenterOfMass(entity, forceType, x, y, z, p5, isDirectionRel, isForceRel, p8);
}