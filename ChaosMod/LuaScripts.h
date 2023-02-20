#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace LuaScripts
{
	void Load();
	void Unload();

	std::vector<std::string> GetEffectIds();

	enum class ExecuteFuncType
	{
		Start,
		Stop,
		Tick,
	};
	void Execute(const std::string &effectId, ExecuteFuncType funcType);

	void RegisterScriptRawTemporary(std::string scriptName, std::string script);
}