#pragma once

#include <string>
#include <string_view>

namespace Hooks
{
	void AddCustomLabel(std::string_view label, const std::string &text);
	void ClearCustomLabels();
}