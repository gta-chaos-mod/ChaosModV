#pragma once

#include "Effects/EffectData.h"
#include "Effects/EffectIdentifier.h"

#include <functional>
#include <unordered_map>

using size_t = unsigned long long;

struct EffectData;

class EffectsIdentifierHasher
{
  public:
	size_t operator()(const EffectIdentifier &effectIdentifier) const
	{
		return std::hash<std::string_view>()(effectIdentifier.GetEffectId());
	}
};

inline std::unordered_map<EffectIdentifier, EffectData, EffectsIdentifierHasher> g_dictEnabledEffects;