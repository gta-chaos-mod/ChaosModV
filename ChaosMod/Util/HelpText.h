#pragma once

#include "Components/HelpTextQueue.h"

#include "Memory/Hooks/GetLabelTextHook.h"

#include <string_view>

__forceinline void DisplayHelpText(const std::string &text, std::uint8_t durationSecs = 0)
{
	Hooks::AddCustomLabel(text, text);

	if (durationSecs == 0)
	{
		if (!IS_HELP_MESSAGE_BEING_DISPLAYED()
		    || (BEGIN_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED(text.data()),
		        END_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED(0)))
		{
			BEGIN_TEXT_COMMAND_DISPLAY_HELP(text.data());
			END_TEXT_COMMAND_DISPLAY_HELP(0, false, false, 0);
		}
	}
	else if (ComponentExists<HelpTextQueue>())
	{
		GetComponent<HelpTextQueue>()->DisplayLabel(text, durationSecs);
	}
}