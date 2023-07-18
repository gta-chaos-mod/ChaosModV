#pragma once

#include <string_view>
#include <unordered_map>

enum class EffectCategory
{
	None,
	Shader,
	Timecycle,
	Gravity,
	Pitch,
	Camera,
	TrafficColor,
	Movement
};

inline const std::unordered_map<std::string_view, EffectCategory> g_NameToEffectCategory {
	{ "None", EffectCategory::None },
	{ "Shader", EffectCategory::Shader },
	{ "Screen", EffectCategory::Timecycle },
	{ "Gravity", EffectCategory::Gravity },
	{ "Pitch", EffectCategory::Pitch },
	{ "Camera", EffectCategory::Camera },
	{ "TrafficColor", EffectCategory::TrafficColor },
	{ "Movement", EffectCategory::Movement },
};