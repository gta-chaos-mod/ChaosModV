#pragma once

#include "EffectIdentifier.h"

#include <unordered_map>

struct EffectData;

struct EffectsIdentifierHasher
{
    std::size_t operator()(const EffectIdentifier& effectIdentifier) const
    {
        std::size_t hash1 = std::hash<int>()(effectIdentifier.GetEffectType());
        std::size_t hash2 = std::hash<std::string>()(effectIdentifier.GetScriptId());

        return hash1 ^ hash2;
    }
};


inline std::unordered_map<EffectIdentifier, EffectData, EffectsIdentifierHasher> g_enabledEffects;