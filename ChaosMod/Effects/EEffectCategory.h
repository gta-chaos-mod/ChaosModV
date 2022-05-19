#pragma once

#include <string_view>
#include <unordered_map>

enum class EEffectCategory
{
	None,
	Shader,
	Timecycle,
	Gravity,
	Pitch,
	Camera,
	TrafficColor
};

inline const std::unordered_map<std::string_view, EEffectCategory> g_dictNameToEffectCategory {
	{ "None", EEffectCategory::None },
	{ "Shader", EEffectCategory::Shader },
	{ "Screen", EEffectCategory::Timecycle },
	{ "Gravity", EEffectCategory::Gravity },
	{ "Pitch", EEffectCategory::Pitch },
	{ "Camera", EEffectCategory::Camera },
	{ "TrafficColor", EEffectCategory::TrafficColor },
};