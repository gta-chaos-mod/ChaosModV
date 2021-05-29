#pragma once

#include "EffectIdentifier.h"

#include <unordered_map>

using size_t = unsigned long long;

struct EffectData;

class EffectsIdentifierHasher
{
public:
    size_t operator()(const EffectIdentifier& effectIdentifier) const
    {
        size_t ullHash1 = std::hash<int>()(effectIdentifier.GetEffectType());
        size_t ullHash2 = std::hash<std::string>()(effectIdentifier.GetScriptId());

        return ullHash1 ^ ullHash2;
    }
};

inline std::unordered_map<EffectIdentifier, EffectData, EffectsIdentifierHasher> g_EnabledEffects;