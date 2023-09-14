#pragma once

#include "Components/Component.h"

#include "Memory/Hooks/ScriptThreadRunHook.h"

#include <map>
#include <vector>

enum class GlobalPatternIdiom : uint8_t
{
	GLOBAL_U16,
	GLOBAL_U24
};

class Globals : public Component
{
  public:
	struct GlobalRegistration
	{
	  public:
		std::string m_Name;
		std::string m_ScriptName;
		std::string m_Pattern;
		int m_Offset;
		int m_Index = -1;
		int m_Handle;
		GlobalPatternIdiom m_PatternIdiom;

		GlobalRegistration(std::string name, std::string scriptName, std::string pattern, int offset,
		                   GlobalPatternIdiom patternIdiom, int handle)
		    : m_Name(name),
		      m_ScriptName(scriptName),
		      m_Pattern(pattern),
		      m_Offset(offset),
		      m_PatternIdiom(patternIdiom),
		      m_Handle(handle)
		{
		}
	};

  private:
	static inline std::vector<GlobalRegistration> ms_Globals = {};

	CHAOS_EVENT_LISTENER(Hooks::OnScriptThreadRun) m_SearchGlobalsListener;

	static bool GetGlobalWhere()
	{
		return false;
	}

  protected:
	Globals();

  public:
	static void SetGlobal(GlobalRegistration &reg, int index)
	{
		reg.m_Index = index;
		ms_Globals.push_back(reg);
	}

	template <typename T> static T *GetGlobalAddr(int handle)
	{
		auto it = std::find_if(ms_Globals.begin(), ms_Globals.end(),
		                       [&](GlobalRegistration &reg) { return reg.m_Handle == handle; });

		if (it == ms_Globals.end())
		{
			return nullptr;
		}

		return reinterpret_cast<T *>(Memory::GetGlobalPtr(it->m_Index));
	}

	// used by RegisterGlobal to check if it wasn't already registered
	static bool GlobalFoundName(std::string name)
	{
		return std::find_if(ms_Globals.begin(), ms_Globals.end(),
		                    [&](GlobalRegistration &reg) { return reg.m_Name == name; })
		    != ms_Globals.end();
	}

	static bool GlobalFound(int handle)
	{
		return std::find_if(ms_Globals.begin(), ms_Globals.end(),
		                    [&](GlobalRegistration &reg) { return reg.m_Handle == handle; })
		    != ms_Globals.end();
	}

	static bool Searching(int handle);
	static int RegisterGlobal(std::string name, std::string scriptName, std::string pattern, int patternOffset,
	                          GlobalPatternIdiom patternIdiom);

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};