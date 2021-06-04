#pragma once

#include "../vendor/scripthookv/inc/natives.h"

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

// Thanks to menyoo!
inline _NODISCARD Hash GET_HASH_KEY(const char* string)
{
	int length = strlen(string);

	DWORD hash, i;
	for (hash = i = 0; i < length; ++i)
	{
		hash += tolower(string[i]);
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);

	return hash;
}

inline void SET_ENTITY_AS_NO_LONGER_NEEDED(Entity* entity)
{
	SET_ENTITY_AS_MISSION_ENTITY(*entity, true, true);
	invoke<Void>(0xB736A491E64A32CF, entity); // orig native
}

inline void SET_OBJECT_AS_NO_LONGER_NEEDED(Object* prop)
{
	SET_ENTITY_AS_NO_LONGER_NEEDED(prop);
}

inline void SET_PED_AS_NO_LONGER_NEEDED(Ped* ped)
{
	SET_ENTITY_AS_NO_LONGER_NEEDED(ped);
}

inline void SET_VEHICLE_AS_NO_LONGER_NEEDED(Vehicle* veh)
{
	SET_ENTITY_AS_NO_LONGER_NEEDED(veh);
}