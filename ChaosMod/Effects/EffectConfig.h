#pragma once

#include "Effects/EffectCategory.h"
#include "Effects/EffectData.h"
#include "Effects/EffectGroups.h"
#include "Effects/EffectIdentifier.h"
#include "Effects/EffectTimedType.h"
#include "Effects/EffectsInfo.h"

#include "Util/OptionsFile.h"

#include <string>
#include <vector>

enum EffectType : int;
struct EffectData;

namespace EffectConfig
{
	inline size_t GetNextDelimiterOffset(const std::string &input)
	{
		bool isInQuotes = false;
		if (input.length() > 0)
		{
			for (size_t i = 0; i < input.length(); i++)
			{
				if (input[i] == '\"')
				{
					isInQuotes = !isInQuotes;
				}
				else if (!isInQuotes && input[i] == ',')
				{
					return i;
				}
			}
		}
		return -1;
	}

	inline void ReadConfig(const char *configPath, auto &out, const char *compatConfigPath = nullptr)
	{
		OptionsFile effectsFile(configPath, compatConfigPath);

		for (auto &[effectId, effectInfo] : g_EffectsMap)
		{
			// Default EffectData values
			// Enabled, TimedType, CustomTime (-1 = Disabled), Weight, Permanent, ExcludedFromVoting, "Dummy for
			// name-override", Shortcut
			std::vector<int> values = { true, static_cast<int>(EffectTimedType::Unk), -1, 5, false, false, 0, 0 };
			// HACK: Store EffectCustomName seperately
			std::string valueEffectName;

			auto value = effectsFile.ReadValueString(std::string(effectId));
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
						{
							split = split.substr(1, split.size() - 2);
						}
						// Names can't be "0" to support older configs
						if (!split.empty() && split != "0")
						{
							valueEffectName = split;
						}
					}
					else
					{
						const auto &split = value.substr(0, splitIndex);

						Util::TryParse<int>(split, values[j]);
					}

					if (splitIndex == value.npos)
					{
						break;
					}

					value      = value.substr(splitIndex + 1);
					splitIndex = GetNextDelimiterOffset(value);
				}
			}

			if (!values[0]) // enabled == false
			{
				continue;
			}

			EffectData effectData;
			if (!effectInfo.IsTimed)
			{
				effectData.TimedType = EffectTimedType::NotTimed;
			}
			else if (values[4])
			{
				effectData.TimedType = EffectTimedType::Permanent;
			}
			else if (values[2] > -1)
			{
				effectData.TimedType  = EffectTimedType::Custom;
				effectData.CustomTime = values[2];
			}
			else
			{
				effectData.TimedType = static_cast<EffectTimedType>(
				    static_cast<EffectTimedType>(values[1]) == EffectTimedType::Unk ? effectInfo.IsShortDuration
				                                                                    : values[1]);
			}

			effectData.WeightMult = values[3];
			effectData.Weight     = effectData.WeightMult; // Set initial effect weight to WeightMult
			effectData.SetAttribute(EffectAttributes::ExcludedFromVoting, values[5]);
			effectData.SetAttribute(EffectAttributes::IsMeta, effectInfo.ExecutionType == EffectExecutionType::Meta);
			effectData.Name = effectInfo.Name;
			effectData.SetAttribute(EffectAttributes::HideRealNameOnStart, effectInfo.HideRealNameOnStart);
#ifdef _DEBUG
			effectData.ShortcutKeycode = effectInfo.DebugShortcutKeycode ? effectInfo.DebugShortcutKeycode : values[7];
#else
			effectData.ShortcutKeycode = values[7];
#endif
			if (!valueEffectName.empty())
			{
				effectData.SetAttribute(EffectAttributes::HasCustomName, true);
				effectData.CustomName = valueEffectName;
			}
			effectData.Id             = effectInfo.Id;
			effectData.EffectCategory = effectInfo.EffectCategory;

			for (auto effectType : effectInfo.IncompatibleWith)
			{
				effectData.IncompatibleIds.push_back(g_EffectsMap.at(effectType).Id);
			}

			if (effectInfo.EffectGroupType != EffectGroupType::None)
			{
				effectData.GroupType = g_EffectGroups.find(g_EffectTypeToGroup.at(effectInfo.EffectGroupType))->first;
				g_EffectGroups[effectData.GroupType].MemberCount++;
			}

			out.emplace(EffectIdentifier(std::string(effectId)), effectData);
		}
	}
}
