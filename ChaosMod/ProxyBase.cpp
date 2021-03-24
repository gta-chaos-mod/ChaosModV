#include "stdafx.h"

#include "ProxyBase.h"

#define BUFFER_SIZE 256

ProxyBase::~ProxyBase()
{
	if (m_pipeHandle != INVALID_HANDLE_VALUE)
	{
		DisconnectNamedPipe(m_pipeHandle);
		CloseHandle(m_pipeHandle);
	}
}

void ProxyBase::Initialize(LPCSTR pipeName)
{
	m_pipeHandle = CreateNamedPipe(pipeName, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT,
		1, BUFFER_SIZE, BUFFER_SIZE, 0, NULL);

	if (m_pipeHandle == INVALID_HANDLE_VALUE)
	{
		ErrorOutWithMsg("Error while creating a named pipe. This is not something that should ever happen.");

		return;
	}

	ConnectNamedPipe(m_pipeHandle, NULL);
}

void ProxyBase::PullMainTickProccess(DWORD64 curTick, bool bShowIngameHud) 
{
	if (m_lastVotesFetchTime < curTick - 500)
	{
		m_lastVotesFetchTime = curTick;

		if (m_isVotingRunning && m_enableChanceSystem && !m_enablePollVoting && bShowIngameHud)
		{
			// Get current vote status to display procentages on screen
			SendToPipe("getcurrentvotes");
		}
	}

	char buffer[BUFFER_SIZE];
	DWORD bytesRead;
	if (!ReadFile(m_pipeHandle, buffer, BUFFER_SIZE, &bytesRead, NULL))
	{
		while (GetLastError() == ERROR_IO_PENDING)
		{
			WAIT(0);
		}
	}

	if (bytesRead > 0)
	{
		if (!HandleMsg(std::string(buffer)))
		{
			return;
		}
	}

	if (g_effectDispatcher->GetRemainingTimerTime() <= 1 && !m_hasReceivedResult)
	{
		// Get vote result 1 second before effect is supposed to dispatch

		if (m_isVotingRunning)
		{
			m_isVotingRunning = false;

			if (!m_noVoteRound)
			{
				SendToPipe("getvoteresult");
			}
		}
	}
	else if (g_effectDispatcher->ShouldDispatchEffectNow())
	{
		// End of voting round; dispatch resulted effect

		if (m_noVoteRound)
		{
			g_effectDispatcher->DispatchRandomEffect();
			g_effectDispatcher->ResetTimer();

			if (!m_enablePollVoting)
			{
				m_noVoteRound = false;
			}
		}
		else
		{
			// Should be random effect voteable, so just dispatch random effect
			if (m_chosenEffectIdentifier->GetEffectType() == _EFFECT_ENUM_MAX && m_chosenEffectIdentifier->GetScriptId().empty())
			{
				g_effectDispatcher->DispatchRandomEffect();
			}
			else
			{
				g_effectDispatcher->DispatchEffect(*m_chosenEffectIdentifier);
			}
			g_effectDispatcher->ResetTimer();
		}

		if (g_metaInfo.AdditionalEffectsToDispatch > 0) 
		{
			for (int i = 0; i < g_metaInfo.AdditionalEffectsToDispatch; i++)
			{
				g_effectDispatcher->DispatchRandomEffect();
			}
		}

		m_isVotingRoundDone = true;
	}
	else if (!m_isVotingRunning && m_receivedFirstPing && (m_SecsBeforeVoting == 0 || g_effectDispatcher->GetRemainingTimerTime() <= m_SecsBeforeVoting) && m_isVotingRoundDone)
	{
		// New voting round

		m_isVotingRunning = true;
		m_hasReceivedResult = false;
		m_isVotingRoundDone = false;

		m_chosenEffectIdentifier = std::make_unique<EffectIdentifier>(_EFFECT_ENUM_MAX);

		if (m_enablePollVoting)
		{
			m_noVoteRound = !m_noVoteRound;
		}

		if (m_noVoteRound)
		{
			SendToPipe("novoteround");

			return;
		}

		m_effectChoices.clear();
		std::unordered_map<EffectIdentifier, EffectData, EffectsIdentifierHasher> choosableEffects;
		for (auto& pair : g_enabledEffects)
		{
			const EffectIdentifier& effectIdentifier = pair.first;
			EffectData& effectData = pair.second;

			if (effectData.TimedType != EffectTimedType::TIMED_PERMANENT && !effectData.IsMeta && !effectData.ExcludedFromVoting)
			{
				choosableEffects.emplace(effectIdentifier, effectData);
			}
		}

		for (int i = 0; i < 4; i++)
		{
			// 4th voteable is for random effect (if enabled)
			if (i == 3)
			{
				if (m_enableRandomEffectVoteable)
				{
					m_effectChoices.push_back(std::make_unique<ChoosableEffect>(EFFECT_NOTHING /* Just give it some random effect */, "Random Effect", !m_alternatedVotingRound ? 4 : 8));
				}

				break;
			}

			int effectsTotalWeight = 0;
			for (const auto& pair : choosableEffects)
			{
				effectsTotalWeight += pair.second.Weight;
			}

			int index = g_random.GetRandomInt(0, effectsTotalWeight);

			int addedUpWeight = 0;
			std::unique_ptr<ChoosableEffect> targetChoice;

			for (auto& pair : choosableEffects)
			{
				EffectData& effectData = pair.second;

				if (effectData.TimedType == EffectTimedType::TIMED_PERMANENT)
				{
					continue;
				}

				addedUpWeight += effectData.Weight;

				if (index <= addedUpWeight)
				{
					// Set weight of this effect 0, EffectDispatcher::DispatchEffect will increment it immediately by EffectWeightMult
					effectData.Weight = 0;

					targetChoice = std::make_unique<ChoosableEffect>(pair.first, effectData.HasCustomName ? effectData.CustomName : effectData.Name,
						!m_alternatedVotingRound
							? i + 1
							: m_enableRandomEffectVoteable
								? i + 5
								: i + 4
					);
					break;
				}
			}

			EffectIdentifier effectIdentifier = targetChoice->EffectIdentifier;

			m_effectChoices.push_back(std::move(targetChoice));
			choosableEffects.erase(effectIdentifier);
		}

		std::ostringstream oss;
		oss << "vote";
		for (const std::unique_ptr<ChoosableEffect>& choosableEffect : m_effectChoices)
		{
			oss << ":" << choosableEffect->EffectName;
		}
		SendToPipe(oss.str());

		m_alternatedVotingRound = !m_alternatedVotingRound;
	}

	if (m_isVotingRunning && !m_noVoteRound && !m_enablePollVoting && bShowIngameHud)
	{
		// Print voteables on screen

		// Count total votes if chance system is enabled
		int totalVotes = 0;
		if (m_enableChanceSystem)
		{
			for (const std::unique_ptr<ChoosableEffect>& choosableEffect : m_effectChoices)
			{
				int chanceVotes = choosableEffect->ChanceVotes + (m_enableVotingChanceSystemRetainChance ? 1 : 0);

				totalVotes += chanceVotes;
			}
		}

		float y = .1f;
		for (const std::unique_ptr<ChoosableEffect>& choosableEffect : m_effectChoices)
		{
			std::ostringstream oss;
			oss << choosableEffect->Match << ": " << choosableEffect->EffectName;

			// Also show chance percentages if chance system is enabled
			if (m_enableChanceSystem)
			{
				float percentage;
				if (totalVotes == 0)
				{
					percentage = 100 / m_effectChoices.size() * .01f;
				}
				else
				{
					int chanceVotes = choosableEffect->ChanceVotes + (m_enableVotingChanceSystemRetainChance ? 1 : 0);

					percentage = !chanceVotes ? .0f : std::roundf(static_cast<float>(chanceVotes) / static_cast<float>(totalVotes) * 100.f) / 100.f;
				}

				oss << " (" << percentage * 100.f << "%)";
			}

			oss << std::endl;

			DrawScreenText(oss.str(), { .95f, y }, .41f, { 210, 210, 210 }, true, ScreenTextAdjust::RIGHT, { .0f, .95f });

			y += .05f;
		}
	}
}

bool ProxyBase::HandleMsg(const std::string& msg)
{
	if (msg == "ping")
	{
		m_lastPing = GetTickCount64();
		m_noPingRuns = 0;
		m_receivedFirstPing = true;
	}
	else if (msg._Starts_with("voteresult"))
	{
		int result = std::stoi(msg.substr(msg.find(":") + 1));

		m_hasReceivedResult = true;

		// If random effect voteable (result == 3) won, dispatch random effect later
		m_chosenEffectIdentifier = std::make_unique<EffectIdentifier>(result == 3 ? _EFFECT_ENUM_MAX : m_effectChoices[result]->EffectIdentifier);
	}
	else if (msg._Starts_with("currentvotes"))
	{
		std::string valuesStr = msg.substr(msg.find(":") + 1);

		int splitIndex = valuesStr.find(":");
		for (int i = 0; ; i++)
		{
			const std::string& split = valuesStr.substr(0, splitIndex);

			TryParse<int>(split, m_effectChoices[i]->ChanceVotes);

			valuesStr = valuesStr.substr(splitIndex + 1);

			if (splitIndex == valuesStr.npos)
			{
				break;
			}

			splitIndex = valuesStr.find(":");
		}
	}
	else
	{
		return OnMessageUnhandled(msg);
	}

	return true;
}

void ProxyBase::SendToPipe(std::string&& msg)
{
	msg += "\n";
	WriteFile(m_pipeHandle, msg.c_str(), msg.length(), NULL, NULL);
}

void ProxyBase::ErrorOutWithMsg(const std::string&& msg)
{
	MessageBox(NULL, msg.c_str(), "ChaosModV Error", MB_OK | MB_ICONERROR);

	DisconnectNamedPipe(m_pipeHandle);
	CloseHandle(m_pipeHandle);
	m_pipeHandle = INVALID_HANDLE_VALUE;

	g_effectDispatcher->OverrideTimerDontDispatch(false);
	m_enableVoting = false;
}