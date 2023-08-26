#pragma once

#include "Components/Component.h"

#include "Memory/Hooks/ScriptThreadRunHook.h"

#include <map>
#include <vector>

enum GlobalPatternIdiom : uint8_t
{
	GLOBAL_U16,
	GLOBAL_U24
};

struct GlobalRegistration
{
  public:
	std::string m_Name;
	std::string m_ScriptName;
	std::string m_Pattern;
	int m_Offset;
	GlobalPatternIdiom m_PatternIdiom;

	GlobalRegistration(std::string name, std::string scriptName, std::string pattern, int offset,
	                   GlobalPatternIdiom patternIdiom)
	    : m_Name(name), m_ScriptName(scriptName), m_Pattern(pattern), m_Offset(offset), m_PatternIdiom(patternIdiom)
	{
	}
};

class Globals : public Component
{
  private:
	static inline std::map<std::string, int> ms_GlobalsIndexMap = {};

	CHAOS_EVENT_LISTENER(Hooks::OnScriptThreadRun) m_SearchGlobalsListener;

  protected:
	Globals();

  public:
	static inline std::vector<GlobalRegistration> m_GlobalsRegistration;

	static void SetGlobalIndex(std::string name, int index)
	{
		m_GlobalsIndexMap.emplace(name, index);
	}

	template <typename T> static T *GetGlobalAddr(std::string name)
	{
		if (!m_GlobalsIndexMap.contains(name))
		{
			return nullptr;
		}

		auto it = m_GlobalsIndexMap.at(name);

		return reinterpret_cast<T *>(Memory::GetGlobalPtr(it));
	}

	static bool GlobalExists(std::string name)
	{
		return m_GlobalsIndexMap.contains(name);
	}

	static void RegisterGlobal(std::string name, std::string scriptName, std::string pattern, int patternOffset,
	                           GlobalPatternIdiom patternIdiom);

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};

#define REGISTER_GLOBAL(name, scriptName, pattern, patternOffset, patternIdiom) \
	Globals::RegisterGlobal(name, scriptName, pattern, patternOffset, patternIdiom);