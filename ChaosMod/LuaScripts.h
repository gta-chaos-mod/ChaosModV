#pragma once

#include <vector>
#include <string>

#define _NODISCARD [[nodiscard]]

namespace LuaScripts
{
	void Load();
	void Unload();
	_NODISCARD std::vector<std::string> GetScriptIds();
	void Execute(const std::string& szScriptId, const char* szFuncName);
}