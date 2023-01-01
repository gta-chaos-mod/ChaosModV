#pragma once

#include "Natives.h"

#include "Util/Script.h"

inline void LoadModel(Hash ulModel)
{
	if (IS_MODEL_VALID(ulModel))
	{
		REQUEST_MODEL(ulModel);
		while (!HAS_MODEL_LOADED(ulModel))
		{
			WAIT(0);
		}
	}
}

inline void UnloadModel(Hash ulHash)
{
	if (IS_MODEL_VALID(ulHash))
	{
		SET_MODEL_AS_NO_LONGER_NEEDED(ulHash);
	}
}