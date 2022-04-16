#pragma once

#include "Util/OptionsFile.h"
#include "Lib/json.hpp"
#include <iostream>
#include <fstream>

enum EEffectType : int;
struct EffectData;

namespace EffectConfig
{
	inline size_t GetNextDelimiterOffset(std::string input)
	{
		bool isInQuotes = false;
		if (input.length() > 0)
		{
			for (size_t curIdx = 0; curIdx < input.length(); curIdx++)
			{
				if (input[curIdx] == '\"')
				{
					isInQuotes = !isInQuotes;
				}
				else if (!isInQuotes && input[curIdx] == ',')
				{
					return curIdx;
				}
			}
		}
		return -1;
	}

	inline void ReadConfig(const char* szConfigPath, auto& out)
	{

		std::ifstream configFile(szConfigPath);
		nlohmann::json configJson;
		configFile >> configJson;
		configFile.close();

		struct ParsedEffect
		{
			std::string name;
			bool enabled;
			int timeMode = static_cast<int>(EEffectTimedType::Unk);
			int customTime = -1;
			int weightMultiplier = 5;
			bool permanent = false;
			bool excludedFromVoting = false;
			std::string customName = "";
			int shortcutKey = 0;

			static ParsedEffect fromJson(nlohmann::json from)
			{
				return ParsedEffect{
					from["name"].get<std::string>(),
					from["enabled"].get<bool>(),
					from["timeMode"].get<int>(),
					from["customTime"].get<int>(),
					from["weightMultiplier"].get<int>(),
					from["permanent"].get<bool>(),
					from["excludeFromVoting"].get<bool>(),
					from["customName"].get<std::string>(),
					from["shortcutKey"].get<int>()
				};
			}
		};
		std::map<std::string, ParsedEffect> allParsedEffects;
		if (!configJson.is_null())
		{
			for (auto item : configJson)
			{
				ParsedEffect eff = ParsedEffect::fromJson(item);
				allParsedEffects.emplace(eff.name, eff);
			}
		}


		for (int i = 0; i < _EFFECT_ENUM_MAX; i++)
		{
			EEffectType effectType = static_cast<EEffectType>(i);
			const EffectInfo& effectInfo = g_dictEffectsMap.at(effectType);
			// Default EffectData values
			ParsedEffect currentEffect;
			auto pos = allParsedEffects.find(effectInfo.Id);
			if (pos == allParsedEffects.end())
			{
				currentEffect = ParsedEffect();
			}
			else
			{
				currentEffect = pos->second;
			}


			if (!currentEffect.enabled) // enabled == false
			{
				if (effectInfo.EEffectGroupType != EEffectGroupType::None)
				{
					g_dictCurrentEffectGroupMemberCount[effectInfo.EEffectGroupType]--;
				}

				continue;
			}

			EffectData effectData;
			if (!effectInfo.IsTimed)
			{
				effectData.TimedType = EEffectTimedType::NotTimed;
			}
			else if (currentEffect.permanent)
			{
				effectData.TimedType = EEffectTimedType::Permanent;
			}
			else if (currentEffect.customTime > -1)
			{
				effectData.TimedType = EEffectTimedType::Custom;
				effectData.CustomTime = currentEffect.customTime;
			}
			else
			{
				effectData.TimedType = static_cast<EEffectTimedType>(static_cast<EEffectTimedType>(currentEffect.timeMode) == EEffectTimedType::Unk ? effectInfo.IsShortDuration : currentEffect.timeMode);
			}

			effectData.WeightMult = currentEffect.weightMultiplier;
			effectData.Weight = effectData.WeightMult; // Set initial effect weight to WeightMult
			effectData.ExcludedFromVoting = currentEffect.excludedFromVoting;
			effectData.IsMeta = effectInfo.ExecutionType == EEffectExecutionType::Meta;
			effectData.Name = effectInfo.Name;
			effectData.Shortcut = currentEffect.shortcutKey;
			if (!currentEffect.customName.empty())
			{
				effectData.HasCustomName = true;
				effectData.CustomName = currentEffect.customName;
			}
			effectData.Id = effectInfo.Id;

			for (EEffectType effectType : effectInfo.IncompatibleWith)
			{
				effectData.IncompatibleIds.push_back(g_dictEffectsMap.at(effectType).Id);
			}

			effectData.EEffectGroupType = effectInfo.EEffectGroupType;

			out.emplace(effectType, effectData);
		}
	}


	inline void OldReadConfig(const char* szConfigPath, auto& out)
	{
		OptionsFile effectsFile(szConfigPath);

		for (int i = 0; i < _EFFECT_ENUM_MAX; i++)
		{
			EEffectType effectType = static_cast<EEffectType>(i);
			const EffectInfo& effectInfo = g_dictEffectsMap.at(effectType);
			// Default EffectData values
			// Enabled, TimedType, CustomTime (-1 = Disabled), Weight, Permanent, ExcludedFromVoting, "Dummy for name-override", Shortcut
			std::vector<int> rgValues{ true, static_cast<int>(EEffectTimedType::Unk), -1, 5, false, false, 0, 0};
			// HACK: Store EffectCustomName seperately
			std::string szValueEffectName;

			std::string szValue = effectsFile.ReadValueString(effectInfo.Id);

			if (!szValue.empty())
			{
				size_t ullSplitIndex = GetNextDelimiterOffset(szValue);
				for (int j = 0; ; j++)
				{
					// Effect-Name override
					if (j == 6)
					{
						std::string szSplit = szValue.substr(0, ullSplitIndex);
						// Trim surrounding quotations
						if (szSplit.length() >= 2 && szSplit[0] == '\"' && szSplit[szSplit.length() -1] == '\"') {
							szSplit = szSplit.substr(1, szSplit.size() - 2);
						}
						// Names can't be "0" to support older configs
						if (!szSplit.empty() && szSplit != "0")
						{
							szValueEffectName = szSplit;
						}
					}
					else
					{
						const std::string& szSplit = szValue.substr(0, ullSplitIndex);

						Util::TryParse<int>(szSplit, rgValues[j]);
					}

					if (ullSplitIndex == szValue.npos)
					{
						break;
					}

					szValue = szValue.substr(ullSplitIndex + 1);
					ullSplitIndex = GetNextDelimiterOffset(szValue);
				}
			}

			if (!rgValues[0]) // enabled == false
			{
				if (effectInfo.EEffectGroupType != EEffectGroupType::None)
				{
					g_dictCurrentEffectGroupMemberCount[effectInfo.EEffectGroupType]--;
				}

				continue;
			}

			EffectData effectData;
			if (!effectInfo.IsTimed)
			{
				effectData.TimedType = EEffectTimedType::NotTimed;
			}
			else if (rgValues[4])
			{
				effectData.TimedType = EEffectTimedType::Permanent;
			}
			else if (rgValues[2] > -1)
			{
				effectData.TimedType = EEffectTimedType::Custom;
				effectData.CustomTime = rgValues[2];
			}
			else
			{
				effectData.TimedType = static_cast<EEffectTimedType>(static_cast<EEffectTimedType>(rgValues[1]) == EEffectTimedType::Unk ? effectInfo.IsShortDuration : rgValues[1]);
			}

			effectData.WeightMult = rgValues[3];
			effectData.Weight = effectData.WeightMult; // Set initial effect weight to WeightMult
			effectData.ExcludedFromVoting = rgValues[5];
			effectData.IsMeta = effectInfo.ExecutionType == EEffectExecutionType::Meta;
			effectData.Name = effectInfo.Name;
			effectData.Shortcut = rgValues[7];
			if (!szValueEffectName.empty())
			{
				effectData.HasCustomName = true;
				effectData.CustomName = szValueEffectName;
			}
			effectData.Id = effectInfo.Id;

			for (EEffectType effectType : effectInfo.IncompatibleWith)
			{
				effectData.IncompatibleIds.push_back(g_dictEffectsMap.at(effectType).Id);
			}

			effectData.EEffectGroupType = effectInfo.EEffectGroupType;

			out.emplace(effectType, effectData);
		}
	}
}
