#pragma once

#include "EffectTimedType.h"

#include <string>
#include <vector>

struct EffectData
{
	EffectTimedType TimedType = EffectTimedType::TIMED_UNK;
	int CustomTime = -1;
	int WeightMult = 5;
	int Weight = WeightMult;
	bool ExcludedFromVoting = false;
	std::string Name;
	bool HasCustomName = false;
	std::string CustomName;
	std::string Id;
	std::vector<std::string> IncompatibleIds;
	bool IsMeta = false;
};