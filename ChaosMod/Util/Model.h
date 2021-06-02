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