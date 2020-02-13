#pragma once

#include "nativesNoNamespaces.h"

struct ReturnedEntities
{
	Entity* begin()
	{
		return Entities;
	}

	Entity* end()
	{
		return Entities + Size;
	}

	int Size;
	Entity Entities[512];
};

inline ReturnedEntities GetAllPeds()
{
	static ReturnedEntities returnedEntities;

	static int lastFrame = 0;
	int curFrame = GET_FRAME_COUNT();
	if (lastFrame < curFrame)
	{
		lastFrame = curFrame;

		returnedEntities.Size = worldGetAllPeds(returnedEntities.Entities, 512);
	}

	return returnedEntities;
}

inline ReturnedEntities GetAllVehs()
{
	static ReturnedEntities returnedEntities;

	static int lastFrame = 0;
	int curFrame = GET_FRAME_COUNT();
	if (lastFrame < curFrame)
	{
		lastFrame = curFrame;

		returnedEntities.Size = worldGetAllVehicles(returnedEntities.Entities, 512);
	}

	return returnedEntities;
}

inline ReturnedEntities GetAllProps()
{
	static ReturnedEntities returnedEntities;

	static int lastFrame = 0;
	int curFrame = GET_FRAME_COUNT();
	if (lastFrame < curFrame)
	{
		lastFrame = curFrame;

		returnedEntities.Size = worldGetAllObjects(returnedEntities.Entities, 512);
	}

	return returnedEntities;
}