#pragma once

#include <unordered_map>
#include <string_view>

enum class EEffectCategory
{
    None,
    Shader
};

inline const std::unordered_map<std::string_view, EEffectCategory> g_dictNameToEffectCategory
{
    { "None", EEffectCategory::None },
    { "Shader", EEffectCategory::Shader }
};