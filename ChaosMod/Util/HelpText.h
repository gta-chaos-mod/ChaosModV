#pragma once

#include "Components/HelpTextQueue.h"

#include "Memory/Hooks/GetLabelTextHook.h"

#include <string_view>

__forceinline void DisplayHelpText(const std::string &text, std::uint8_t durationSecs = 0)
{
#define __FILENAME__                                        \
	(strrchr(__FILE__, '\\')  ? strrchr(__FILE__, '\\') + 1 \
	 : strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1  \
	                          : __FILE__)
#define STRINGIFY(a) #a

	auto label = STRINGIFY(__FILENAME__) STRINGIFY(__LINE__);

	Hooks::AddCustomLabel(label, text);

#undef STRINGIFY
#undef __FILENAME__

	if (durationSecs == 0)
	{
		if (!IS_HELP_MESSAGE_BEING_DISPLAYED()
		    || (BEGIN_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED(label),
		        END_TEXT_COMMAND_IS_THIS_HELP_MESSAGE_BEING_DISPLAYED(0)))
		{
			BEGIN_TEXT_COMMAND_DISPLAY_HELP(label);
			END_TEXT_COMMAND_DISPLAY_HELP(0, false, false, 0);
		}
	}
	else if (ComponentExists<HelpTextQueue>())
	{
		GetComponent<HelpTextQueue>()->DisplayLabel(label, durationSecs);
	}
}