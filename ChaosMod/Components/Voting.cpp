#include <stdafx.h>

#include "Voting.h"

#include "Components/EffectDispatchTimer.h"
#include "Components/EffectDispatcher.h"
#include "Components/MetaModifiers.h"
#include "Components/SplashTexts.h"
#include "Effects/EffectData.h"
#include "Effects/EnabledEffects.h"
#include "Util/OptionsManager.h"
#include "Util/ScriptText.h"

#include <json.hpp>

#define BUFFER_SIZE 256
#define VOTING_PROXY_START_ARGS L"chaosmod\\TwitchChatVotingProxy.exe --startProxy"

Voting::Voting(const std::array<BYTE, 3> &textColor) : Component(), m_TextColor(textColor)
{
	m_EnableVoting =
	    g_OptionsManager.GetVotingValue({ "EnableVoting", "EnableTwitchVoting" }, OPTION_DEFAULT_TWITCH_VOTING_ENABLED);
	m_VoteablePrefix = g_OptionsManager.GetVotingValue<std::string>({ "VoteablePrefix" }, "");
}

bool Voting::Init()
{
	if (std::count_if(g_EnabledEffects.begin(), g_EnabledEffects.end(),
	                  [](const auto &pair) { return !pair.second.IsExcludedFromVoting(); })
	    < 3)
	{
		ErrorOutWithMsg("You need at least 3 enabled effects (which are not excluded from voting) to enable voting."
		                " Reverting to normal mode.");

		return false;
	}

	// A previous instance of the voting proxy could still be running, wait for it to release the mutex
	auto mutex = OpenMutex(SYNCHRONIZE, FALSE, L"ChaosModVVotingMutex");
	if (mutex)
	{
		WaitForSingleObject(mutex, INFINITE);
		ReleaseMutex(mutex);
		CloseHandle(mutex);
	}

	m_SecsBeforeVoting = g_OptionsManager.GetVotingValue({ "VotingSecsBeforeVoting", "TwitchVotingSecsBeforeVoting" },
	                                                     OPTION_DEFAULT_TWITCH_SECS_BEFORE_VOTING);

	m_OverlayMode      = g_OptionsManager.GetVotingValue({ "VotingOverlayMode", "TwitchVotingOverlayMode" },
	                                                     static_cast<OverlayMode>(OPTION_DEFAULT_TWITCH_OVERLAY_MODE));

	m_VotingMode       = g_OptionsManager.GetVotingValue({ "VotingChanceSystem", "TwitchVotingChanceSystem" },
	                                                     OPTION_DEFAULT_TWITCH_PROPORTIONAL_VOTING)
	                       ? VotingMode::Percentage
	                       : VotingMode::Majority;
	m_EnableVotingChanceSystemRetainInitialChance =
	    g_OptionsManager.GetVotingValue({ "VotingChanceSystemRetainChance", "TwitchVotingChanceSystemRetainChance" },
	                                    OPTION_DEFAULT_TWITCH_PROPORTIONAL_VOTING_RETAIN_CHANCE);

	m_EnableRandomEffectVoteable = g_OptionsManager.GetVotingValue(
	    { "RandomEffectVoteableEnable", "TwitchRandomEffectVoteableEnable" }, OPTION_DEFAULT_TWITCH_RANDOM_EFFECT);

	STARTUPINFO startupInfo      = {};
	PROCESS_INFORMATION procInfo = {};

	auto str                     = _wcsdup(VOTING_PROXY_START_ARGS);
#ifdef CHAOSDEBUG
	DWORD attributes = 0;
	if (DoesFeatureFlagExist("forcenovotingconsole"))
		attributes = CREATE_NO_WINDOW;

	bool result = CreateProcess(NULL, str, NULL, NULL, TRUE, attributes, NULL, NULL, &startupInfo, &procInfo);
#else
	bool result = CreateProcess(NULL, str, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &procInfo);
#endif
	free(str);

	if (!result)
	{
		ErrorOutWithMsg((std::ostringstream()
		                 << "Error while starting chaosmod/TwitchChatVotingProxy.exe (Error Code: " << GetLastError()
		                 << "). Please verify the file exists. Reverting to normal mode.")
		                    .str());

		return false;
	}

	return true;
}

bool Voting::IsEnabled() const
{
	return m_EnableVoting;
}

VotingMode Voting::GetVotingMode() const
{
	return m_VotingMode;
}

void Voting::HandleMsg(std::string_view message)
{
	if (message == "hello")
	{
		if (!m_ReceivedHello)
		{
			m_ReceivedHello = true;
			m_NoPingRuns    = 0;

			LOG("Received hello from voting pipe");

			if (ComponentExists<EffectDispatchTimer>())
				GetComponent<EffectDispatchTimer>()->SetShouldDispatchEffects(false);

			SendToPipe("hello_back");
		}
	}
	else if (message == "ping")
	{
		m_LastPing   = GetTickCount64();
		m_NoPingRuns = 0;
	}
	else
	{
		auto receivedJSON = nlohmann::json::parse(message);
		if (!receivedJSON.empty())
		{
			std::string identifier = receivedJSON["Identifier"];
			if (identifier == "voteresult")
			{
				int result          = receivedJSON["SelectedOption"];

				m_HasReceivedResult = true;

				// If random effect voteable (result == 3) won, dispatch random effect later
				m_ChosenEffectId =
				    std::make_unique<EffectIdentifier>(result == 3 ? EffectIdentifier() : m_EffectChoices[result]->Id);
			}
			else if (identifier == "currentvotes")
			{
				std::vector<int> options = receivedJSON["Votes"];
				if (options.size() == m_EffectChoices.size())
				{
					for (size_t idx = 0; idx < options.size(); idx++)
					{
						int votes                         = options[idx];
						m_EffectChoices[idx]->ChanceVotes = votes;
					}
				}
			}
			else if (identifier == "error")
			{
				std::string message = receivedJSON["Message"];
				ErrorOutWithMsg(message);
			}
		}
	}
}

std::string Voting::GetPipeJson(std::string_view identifier, std::vector<std::string> params)
{
	nlohmann::json finalJSON;
	finalJSON["Identifier"] = identifier;
	finalJSON["Options"]    = params;
	return finalJSON.dump();
}

void Voting::SendToPipe(std::string_view identifier, std::vector<std::string> params)
{
	auto msg = GetPipeJson(identifier, params);
	msg += "\n";
	WriteFile(m_PipeHandle, msg.c_str(), msg.length(), NULL, NULL);
}

void Voting::ErrorOutWithMsg(std::string_view message)
{
	std::wstring wStr = { message.begin(), message.end() };
	MessageBox(NULL, wStr.c_str(), L"ChaosModV Error", MB_OK | MB_ICONERROR);

	DisconnectNamedPipe(m_PipeHandle);
	CloseHandle(m_PipeHandle);
	m_PipeHandle = INVALID_HANDLE_VALUE;

	if (ComponentExists<EffectDispatchTimer>())
		GetComponent<EffectDispatchTimer>()->SetShouldDispatchEffects(true);

	m_EnableVoting = false;
}

void Voting::OnModPauseCleanup()
{
	if (m_PipeHandle != INVALID_HANDLE_VALUE)
	{
		FlushFileBuffers(m_PipeHandle);
		DisconnectNamedPipe(m_PipeHandle);
		CloseHandle(m_PipeHandle);

		m_PipeHandle = INVALID_HANDLE_VALUE;
	}
}

void Voting::OnRun()
{
	if (!m_EnableVoting || !ComponentExists<EffectDispatcher>() || !ComponentExists<EffectDispatchTimer>())
		return;

	if (!m_HasInitializedVoting)
	{
		// Only initialize voting proxy after we are fully loaded in, otherwise some weird behaviour can occur from the
		// voting proxy, e.g. OnFailureToReceiveJoinConfirmation being raised for whatever reason
		if (GET_IS_LOADING_SCREEN_ACTIVE())
			return;

		m_HasInitializedVoting = true;

		if (!Init())
			return;

		m_PipeHandle = CreateNamedPipe(L"\\\\.\\pipe\\ChaosModVVotingPipe", PIPE_ACCESS_DUPLEX,
		                               PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_NOWAIT, 1, BUFFER_SIZE,
		                               BUFFER_SIZE, 0, NULL);

		if (m_PipeHandle == INVALID_HANDLE_VALUE)
		{
			ErrorOutWithMsg(
			    "Error while creating a named pipe, previous instance of voting proxy might be running. Try "
			    "reloading the mod. Reverting to normal mode.");

			return;
		}

		ConnectNamedPipe(m_PipeHandle, NULL);

		if (m_EnableVoting)
		{
			if ((m_OverlayMode == OverlayMode::OverlayIngame || m_OverlayMode == OverlayMode::OverlayOBS)
			    && ComponentExists<EffectDispatcher>())
				GetComponent<EffectDispatcher>()->EnableEffectTextExtraTopSpace = true;

			if (ComponentExists<SplashTexts>())
				GetComponent<SplashTexts>()->ShowVotingSplash();
		}

		return;
	}

	// Check if there's been no ping for too long and error out
	// Also if the chance system is enabled, get current vote status every second (if shown on screen)
	auto curTick = GetTickCount64();
	if (m_LastPing < curTick - 1000)
	{
		if (m_NoPingRuns++ == 5)
		{
			ErrorOutWithMsg(
			    "Connection to voting proxy process lost. Check chaosmod/chaosproxy.log for more information."
			    " Returning to normal mode.");

			return;
		}

		m_LastPing = curTick;
	}

	if (m_LastVotesFetchTime < curTick - 500)
	{
		m_LastVotesFetchTime = curTick;

		if (m_IsVotingRunning && m_VotingMode == VotingMode::Percentage && m_OverlayMode == OverlayMode::OverlayIngame)
		{
			// Get current vote status to display procentages on screen
			SendToPipe("getcurrentvotes");
		}
	}

	char buffer[BUFFER_SIZE];
	DWORD bytesRead;
	if (!ReadFile(m_PipeHandle, buffer, BUFFER_SIZE, &bytesRead, NULL))
		while (GetLastError() == ERROR_IO_PENDING)
			WAIT(0);

	if (bytesRead > 0)
		HandleMsg(buffer);

	if (!m_ReceivedHello)
		return;

	if (ComponentExists<MetaModifiers>())
	{
		auto newMode = GetComponent<MetaModifiers>()->VotingModeOverride;
		if (m_VotingModeOverride != newMode)
		{
			m_VotingModeOverride = newMode;

			if (newMode == VotingMode::None)
				newMode = m_VotingMode;

			SendToPipe("votingmode", { std::string(newMode.ToString()) });
		}
	}

	if (GetComponent<EffectDispatchTimer>()->GetRemainingTimerTime() <= 1 && !m_HasReceivedResult)
	{
		// Get vote result 1 second before effect is supposed to dispatch

		if (m_IsVotingRunning)
		{
			m_IsVotingRunning = false;

			SendToPipe("getvoteresult");
		}
	}
	else if (GetComponent<EffectDispatchTimer>()->ShouldDispatchEffectNow())
	{
		// End of voting round; dispatch resulting effect

		// Should be random effect voteable, so just dispatch random effect
		if (m_ChosenEffectId->Id().empty())
			GetComponent<EffectDispatcher>()->DispatchRandomEffect();
		else
			GetComponent<EffectDispatcher>()->DispatchEffect(*m_ChosenEffectId);
		GetComponent<EffectDispatchTimer>()->ResetTimer();

		if (ComponentExists<MetaModifiers>())
			for (int i = 0; i < GetComponent<MetaModifiers>()->AdditionalEffectsToDispatch; i++)
				GetComponent<EffectDispatcher>()->DispatchRandomEffect();

		m_IsVotingRoundDone = true;
	}
	else if (!m_IsVotingRunning && m_ReceivedHello
	         && (m_SecsBeforeVoting == 0
	             || GetComponent<EffectDispatchTimer>()->GetRemainingTimerTime() <= m_SecsBeforeVoting)
	         && m_IsVotingRoundDone)
	{
		// New voting round

		m_IsVotingRunning   = true;
		m_HasReceivedResult = false;
		m_IsVotingRoundDone = false;

		m_ChosenEffectId    = std::make_unique<EffectIdentifier>();

		m_EffectChoices.clear();

		const auto &filteredEffects = GetFilteredEnabledEffects();
		std::vector<EffectData *> choosableEffects;
		choosableEffects.reserve(filteredEffects.size());
		for (auto &effectData : filteredEffects)
		{
			if (!effectData->IsMeta() && !effectData->IsExcludedFromVoting() && !effectData->IsUtility()
			    && !effectData->IsHidden())
			{
				choosableEffects.push_back(effectData);
			}
		}

		for (int idx = 0; idx < 4; idx++)
		{
			// 4th voteable is for random effect (if enabled)
			if (idx == 3)
			{
				if (m_EnableRandomEffectVoteable)
				{
					auto match = (std::ostringstream() << m_VoteablePrefix << (!m_AlternatedVotingRound ? 4 : 8)).str();
					m_EffectChoices.push_back(
					    std::make_unique<ChoosableEffect>(EffectIdentifier(), "Random Effect", match));
				}

				break;
			}

			float totalWeight = 0.f;
			for (const auto &effectData : choosableEffects)
				totalWeight += effectData->GetEffectWeight();

			float chosen = g_RandomNoDeterm.GetRandomFloat(0.f, totalWeight);

			totalWeight  = 0.f;
			for (auto it = choosableEffects.begin(); it != choosableEffects.end();)
			{
				const auto &effectData = *it;

				totalWeight += effectData->GetEffectWeight();
				if (chosen <= totalWeight)
				{
					// Set weight of this effect 0, EffectDispatcher::DispatchEffect will increment it immediately by
					// EffectWeightMult
					effectData->Weight = 0;

					auto match         = (std::ostringstream() << m_VoteablePrefix
                                                       << (!m_AlternatedVotingRound       ? idx + 1
					                                               : m_EnableRandomEffectVoteable ? idx + 5
					                                                                              : idx + 4))
					                 .str();

					m_EffectChoices.push_back(std::make_unique<ChoosableEffect>(
					    effectData->Id, effectData->HasCustomName() ? effectData->CustomName : effectData->Name,
					    match));
					it = choosableEffects.erase(it);
					break;
				}

				it++;
			}
		}

		std::vector<std::string> effectNames;
		for (const auto &pChoosableEffect : m_EffectChoices)
			effectNames.push_back(pChoosableEffect->Name);

		SendToPipe("vote", effectNames);

		m_AlternatedVotingRound = !m_AlternatedVotingRound;
	}

	if (m_IsVotingRunning && m_OverlayMode == OverlayMode::OverlayIngame)
	{
		// Print voteables on screen

		// Count total votes if chance system is enabled
		int totalVotes = 0;
		if (m_VotingMode == VotingMode::Percentage)
		{
			for (const auto &choosableEffect : m_EffectChoices)
			{
				int chanceVotes =
				    choosableEffect->ChanceVotes + (m_EnableVotingChanceSystemRetainInitialChance ? 1 : 0);

				totalVotes += chanceVotes;
			}
		}

		float y = .1f;
		for (const auto &choosableEffect : m_EffectChoices)
		{
			std::ostringstream oss;
			oss << choosableEffect->Match << ": " << choosableEffect->Name;

			// Also show chance percentages if chance system is enabled
			if (m_VotingMode == VotingMode::Percentage)
			{
				float percentage;
				if (totalVotes == 0)
				{
					percentage = 100.f / m_EffectChoices.size() * .01f;
				}
				else
				{
					int chanceVotes =
					    choosableEffect->ChanceVotes + (m_EnableVotingChanceSystemRetainInitialChance ? 1 : 0);

					percentage =
					    !chanceVotes
					        ? .0f
					        : std::roundf(static_cast<float>(chanceVotes) / static_cast<float>(totalVotes) * 100.f)
					              / 100.f;
				}

				oss << " (" << percentage * 100.f << "%)";
			}

			DrawScreenText(oss.str(), { .95f, y }, .41f, { m_TextColor[0], m_TextColor[1], m_TextColor[2] }, true,
			               ScreenTextAdjust::Right, { .0f, .95f }, true);

			y += .05f;
		}
	}
}