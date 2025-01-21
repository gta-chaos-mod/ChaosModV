#pragma once

#include "Effects/Register/RegisteredEffects.h"
#include "Effects/Register/RegisteredEffectsMetadata.h"
#include "Util/File.h"
#include "Util/MacroConcat.h"

#include <functional>

#define CHAOS_VAR static thread_local

#define REGISTER_EFFECT(start, stop, tick, ...)                                   \
	namespace                                                                     \
	{                                                                             \
		RegisterEffect CONCAT(_effect, __LINE__)(start, stop, tick, __VA_ARGS__); \
	}

class RegisterEffect
{
	RegisteredEffect m_RegisteredEffect;

  public:
	RegisterEffect(std::function<void()> start, std::function<void()> stop, std::function<void()> tick,
	               RegisteredEffectMetadata &&effectMetadata)
	    : m_RegisteredEffect(std::string(effectMetadata.Id), start, stop, tick)
	{
		static bool disableEffectRegistration = []()
		{
			return DoesFeatureFlagExist("disablebuiltineffects");
		}();

		if (disableEffectRegistration)
			return;

		g_RegisteredEffects.push_back(m_RegisteredEffect);
		g_RegisteredEffectsMetadata[effectMetadata.Id] = effectMetadata;
	}

	RegisterEffect(const RegisterEffect &)            = delete;
	RegisterEffect &operator=(const RegisterEffect &) = delete;
};
