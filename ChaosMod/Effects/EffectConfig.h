#pragma once

#include "Effects/EEffectCategory.h"
#include "Effects/EEffectTimedType.h"
#include "Effects/EffectData.h"
#include "Effects/EffectGroups.h"
#include "Effects/EffectIdentifier.h"
#include "Effects/EffectsInfo.h"

#include "Util/OptionsFile.h"

#include <string>
#include <vector>

enum EEffectType : int;
struct EffectData;

namespace EffectConfig
{
	inline size_t GetNextDelimiterOffset(const std::string &input)
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

	inline void ReadConfig(const char *szConfigPath, auto &out)
	{
		OptionsFile effectsFile(szConfigPath);

		for (auto &[effectId, effectInfo] : g_dictEffectsMap)
		{
			// Default EffectData values
			// Enabled, TimedType, CustomTime (-1 = Disabled), Weight, Permanent, ExcludedFromVoting, "Dummy for
			// name-override", Shortcut
			std::vector<int> rgValues = { true, static_cast<int>(EEffectTimedType::Unk), -1, 5, false, false, 0, 0 };
			// HACK: Store EffectCustomName seperately
			std::string szValueEffectName;

			auto szValue = effectsFile.ReadValueString(std::string(effectId));
			if (!szValue.empty())
			{
				size_t ullSplitIndex = GetNextDelimiterOffset(szValue);
				for (int j = 0;; j++)
				{
					// Effect-Name override
					if (j == 6)
					{
						std::string szSplit = szValue.substr(0, ullSplitIndex);
						// Trim surrounding quotations
						if (szSplit.length() >= 2 && szSplit[0] == '\"' && szSplit[szSplit.length() - 1] == '\"')
						{
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
						const std::string &szSplit = szValue.substr(0, ullSplitIndex);

						Util::TryParse<int>(szSplit, rgValues[j]);
					}

					if (ullSplitIndex == szValue.npos)
					{
						break;
					}

					szValue       = szValue.substr(ullSplitIndex + 1);
					ullSplitIndex = GetNextDelimiterOffset(szValue);
				}
			}

			if (!rgValues[0]) // enabled == false
			{
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
				effectData.TimedType  = EEffectTimedType::Custom;
				effectData.CustomTime = rgValues[2];
			}
			else
			{
				effectData.TimedType = static_cast<EEffectTimedType>(
					static_cast<EEffectTimedType>(rgValues[1]) == EEffectTimedType::Unk ? effectInfo.IsShortDuration
																						: rgValues[1]);
			}

			effectData.WeightMult = rgValues[3];
			effectData.Weight     = effectData.WeightMult; // Set initial effect weight to WeightMult
			effectData.SetAttribute(EEffectAttributes::ExcludedFromVoting, rgValues[5]);
			effectData.SetAttribute(EEffectAttributes::IsMeta, effectInfo.ExecutionType == EEffectExecutionType::Meta);
			effectData.Name            = effectInfo.Name;
			effectData.ShortcutKeycode = rgValues[7];
			if (!szValueEffectName.empty())
			{
				effectData.SetAttribute(EEffectAttributes::HasCustomName, true);
				effectData.CustomName = szValueEffectName;
			}
			effectData.Id             = effectInfo.Id;
			effectData.EffectCategory = effectInfo.EffectCategory;

			for (auto effectType : effectInfo.IncompatibleWith)
			{
				effectData.IncompatibleIds.push_back(g_dictEffectsMap.at(effectType).Id);
			}

			if (effectInfo.EffectGroupType != EEffectGroupType::None)
			{
				effectData.GroupType =
					g_dictEffectGroups.find(g_dictEffectTypeToGroup.at(effectInfo.EffectGroupType))->first;
				g_dictEffectGroupMemberCount[effectData.GroupType]++;
			}

			out.emplace(EffectIdentifier(std::string(effectId)), effectData);
		}
	}
}
