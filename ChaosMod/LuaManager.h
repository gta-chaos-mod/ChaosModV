#pragma once

#include <vector>
#include <string>

struct LuaScriptInfo
{
	LuaScriptInfo(bool isTimed = false, bool isShortTimed = false) : IsTimed(isTimed), IsShortTimed(isShortTimed)
	{

	}

	const bool IsTimed;
	const bool IsShortTimed;
};

namespace LuaManager
{
	void Load();
	std::vector<std::string> GetScriptNames();
	void Execute(const std::string& scriptName, const char* funcName);
	LuaScriptInfo GetScriptInfo(const std::string& scriptName);
}