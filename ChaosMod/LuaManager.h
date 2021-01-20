#pragma once

#include "Effects/EffectTimedType.h"

#include <vector>
#include <string>

namespace LuaManager
{
	void Load();
	std::vector<std::string> GetScriptIds();
	void Execute(const std::string& scriptId, const char* funcName);
}