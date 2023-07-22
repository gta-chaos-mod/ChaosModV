#pragma once

#include <string_view>

namespace Hooks
{
	void AddCustomLabel(std::string_view label, std::string_view text);
	void ClearCustomLabels();
}