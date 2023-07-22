#include <stdafx.h>

#include "HelpTextQueue.h"

#include "Memory/Hooks/GetLabelTextHook.h"

void HelpTextQueue::DisplayLabel(std::string_view label, std::uint8_t durationSecs)
{
	if (durationSecs == 0)
	{
		return;
	}

	m_HelpTextQueue.emplace(label, durationSecs / 1000.f);
}

void HelpTextQueue::OnModPauseCleanup()
{
	Hooks::ClearCustomLabels();
}

void HelpTextQueue::OnRun()
{
	if (m_HelpTextQueue.empty() || IS_HELP_MESSAGE_BEING_DISPLAYED())
	{
		return;
	}

	auto &helpText = m_HelpTextQueue.front();

	BEGIN_TEXT_COMMAND_DISPLAY_HELP(helpText.Text.data());
	END_TEXT_COMMAND_DISPLAY_HELP(0, false, false, 0);

	if ((helpText.TimerSecs -= GET_FRAME_TIME()) < 0.f)
	{
		m_HelpTextQueue.pop();
	}
}