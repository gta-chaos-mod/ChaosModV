#pragma once

#include "nativesNoNamespaces.h"

#include "Util/Script.h"

inline void LoadModel(Hash model)
{
	if (IS_MODEL_VALID(model))
	{
		REQUEST_MODEL(model);
		while (!HAS_MODEL_LOADED(model))
		{
			WAIT(0);
		}
	}
}