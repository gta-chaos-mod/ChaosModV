#include "stdafx.h"
#include "TwitchVoting.h"

#define URL "https://ducky.rivinshosting.com:8080/chaosmod/poll/new"

TwitchVoting::TwitchVoting(bool enableTwitchVoting, bool twitchVotingNoVoteChance, std::shared_ptr<EffectDispatcher> effectDispatcher,
	std::map<EffectType, std::array<int, 3>> enabledEffects) : m_enableTwitchVoting(enableTwitchVoting), m_twitchVotingNoVoteChance(twitchVotingNoVoteChance),
	m_effectDispatcher(effectDispatcher), m_enabledEffects(enabledEffects)
{
	if (enableTwitchVoting)
	{
		m_effectDispatcher->OverrideTimerDontDispatch(true);
	}
}

void TwitchVoting::Tick()
{
	if (m_effectDispatcher->GetRemainingTimerTime() == 1)
	{
		if (m_isVotingRunning)
		{
			m_isVotingRunning = false;

			// TODO: Get Result
		}
	}
	else if (m_effectDispatcher->ShouldDispatchEffectNow())
	{
		m_effectDispatcher->DispatchRandomEffect();
		m_effectDispatcher->ResetTimer();
	}
	else if (!m_isVotingRunning)
	{
		m_isVotingRunning = false;

		std::map<EffectType, std::array<int, 3>> choosableEffects;
		for (auto pair : m_enabledEffects)
		{
			choosableEffects.emplace(pair);
		}

		for (int i = 0; i < 3; i++)
		{
			int effectsTotalWeight = 0;
			for (auto pair : choosableEffects)
			{
				effectsTotalWeight += pair.second[2];
			}

			int index = Random::GetRandomInt(0, effectsTotalWeight);

			int addedUpWeight = 0;
			EffectType targetEffectType = _EFFECT_ENUM_MAX;
			for (auto pair : choosableEffects)
			{
				addedUpWeight += pair.second[2];

				if (index <= addedUpWeight)
				{
					targetEffectType = pair.first;
					break;
				}
			}

			m_effectChoices[i] = targetEffectType;
		}

		/*CURL* curl = curl_easy_init();
		curl_easy_setopt(curl, CURLOPT_URL, URL);
		curl_easy_setopt(curl, CURLOPT_POST, true);

		std::ostringstream oss;
		oss << "{'votables': ['" << g_effectsMap.at(m_effectChoices[0]).Name << "','" << g_effectsMap.at(m_effectChoices[1]).Name
			<< "','" << g_effectsMap.at(m_effectChoices[2]).Name << "], 'channel_id':'pongo1231'}";

		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, oss.str().c_str());
		curl_easy_perform(curl);

		curl_easy_cleanup(curl);*/
	}
}