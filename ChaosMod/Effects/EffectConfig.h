#pragma once

#include "Effects/EffectData.h"
#include "Effects/EffectGroups.h"
#include "Effects/EffectIdentifier.h"
#include "Effects/EffectTimedType.h"
#include "Effects/Register/RegisteredEffectsMetadata.h"
#include "Util/OptionsFile.h"

#include <string>
#include <vector>

enum EffectType : int;
class EffectData;

namespace EffectConfig
{
	inline size_t GetNextDelimiterOffset(const std::string &input)
	{
		bool isInQuotes = false;
		if (input.length() > 0)
		{
			for (size_t i = 0; i < input.length(); i++)
				if (input[i] == '\"')
					isInQuotes = !isInQuotes;
				else if (!isInQuotes && input[i] == ',')
					return i;
		}
		return -1;
	}

	inline void ReadConfig(const char *configPath, auto &out, std::vector<const char *> compatConfigPaths = {})
	{
		OptionsFile effectsFile(configPath, compatConfigPaths);

		for (auto &[effectId, effectMetadata] : g_RegisteredEffectsMetadata)
		{
			struct ConfigValues
			{
				// Declared as named struct outside the union because MSVC doesn't like default initialization in
				// unnamed structs inside unions
				struct DetailedValues
				{
					alignas(int) bool Enabled              = true;
					alignas(int) EffectTimedType TimedType = EffectTimedType::NotTimed;
					alignas(int) int CustomTime            = 0; // Used to be set to -1 by default so no uint
					alignas(int) uint WeightMult           = 0;
					alignas(int) bool Permanent            = false;
					alignas(int) bool ExcludedFromVoting   = false;
					alignas(int) char Placeholder;
					alignas(int) uint ShortcutKeycode = 0;
				};
				union
				{
					std::array<int, 8> ValuesRaw;
					DetailedValues Values {};
				};
			} configValues;
			// HACK: Store EffectCustomName seperately
			std::string valueEffectName;

			auto value = effectsFile.ReadValueString({ std::string(effectId) });
			if (!value.empty())
			{
				size_t splitIndex = GetNextDelimiterOffset(value);
				for (int j = 0;; j++)
				{
					// Effect-Name override
					if (j == 6)
					{
						auto split = value.substr(0, splitIndex);
						// Trim surrounding quotations
						if (split.length() >= 2 && split[0] == '\"' && split[split.length() - 1] == '\"')
							split = split.substr(1, split.size() - 2);
						// Names can't be "0" to support older configs
						if (!split.empty() && split != "0")
							valueEffectName = split;
					}
					else
					{
						const auto &split = value.substr(0, splitIndex);

						Util::TryParse<int>(split, configValues.ValuesRaw[j]);
					}

					if (splitIndex == value.npos)
						break;

					value      = value.substr(splitIndex + 1);
					splitIndex = GetNextDelimiterOffset(value);
				}
			}

			if (!configValues.Values.Enabled)
				continue;

			EffectData effectData;
			if (!effectMetadata.IsTimed)
			{
				effectData.TimedType = EffectTimedType::NotTimed;
			}
			else if (configValues.Values.Permanent)
			{
				effectData.TimedType = EffectTimedType::Permanent;
			}
			else if (configValues.Values.CustomTime > 0)
			{
				effectData.TimedType  = EffectTimedType::Custom;
				effectData.CustomTime = static_cast<float>(configValues.Values.CustomTime);
			}
			else
			{
				effectData.TimedType =
				    configValues.Values.TimedType == EffectTimedType::NotTimed
				        ? (effectMetadata.IsShortDuration ? EffectTimedType::Short : EffectTimedType::Normal)
				        : configValues.Values.TimedType;
			}

			if (configValues.Values.WeightMult > 0)
				effectData.WeightMult = static_cast<float>(configValues.Values.WeightMult);
			effectData.Weight = effectData.WeightMult; // Set initial effect weight to WeightMult
			effectData.SetAttribute(EffectAttributes::ExcludedFromVoting, configValues.Values.ExcludedFromVoting);
			effectData.SetAttribute(EffectAttributes::IsMeta,
			                        effectMetadata.ExecutionType == EffectExecutionType::Meta);
			effectData.Name = effectMetadata.Name;
			effectData.SetAttribute(EffectAttributes::HideRealNameOnStart, effectMetadata.HideRealNameOnStart);
#ifdef CHAOSDEBUG
			effectData.ShortcutKeycode = effectMetadata.DebugShortcutKeycode ? effectMetadata.DebugShortcutKeycode
			                                                                 : configValues.Values.ShortcutKeycode;
#else
			effectData.ShortcutKeycode = configValues.Values.ShortcutKeycode;
#endif
			if (!valueEffectName.empty())
				effectData.CustomName = valueEffectName;
			effectData.Id            = { std::string(effectMetadata.Id) };
			effectData.Category      = effectMetadata.EffectCategory;
			effectData.ConditionType = effectMetadata.ConditionType;

			for (auto effectType : effectMetadata.IncompatibleWith)
				effectData.IncompatibleIds.insert(std::string(g_RegisteredEffectsMetadata.at(effectType).Id));

			if (effectMetadata.EffectGroupType != EffectGroupType::None)
			{
				effectData.GroupType =
				    g_EffectGroups.find(g_EffectTypeToGroup.at(effectMetadata.EffectGroupType))->first;
				g_EffectGroups[effectData.GroupType].MemberCount++;
			}

			out.emplace(EffectIdentifier(std::string(effectId)), effectData);
		}
	}
}
