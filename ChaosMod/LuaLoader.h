#pragma once

#include <vector>
#include <string>

namespace LuaLoader
{
	void Load();
	const std::vector<std::string>& GetScriptNames();
}