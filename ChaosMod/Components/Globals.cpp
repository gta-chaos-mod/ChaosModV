#include <stdafx.h>

#include "Globals.h"

#include "Lib/scrThread.h"

#include "Memory/Script.h"

std::vector<Globals::GlobalRegistration> ms_GlobalsRegistration = {};

Globals::Globals() : Component()
{
	m_SearchGlobalsListener.Register(
	    Hooks::OnScriptThreadRun,
	    [&](rage::scrThread *thread)
	    {
		    if (ms_GlobalsRegistration.size() <= 0)
		    {
			    return true;
		    }

		    auto match =
		        std::find_if(ms_GlobalsRegistration.begin(), ms_GlobalsRegistration.end(),
		                     [&thread](GlobalRegistration &reg) { return reg.m_ScriptName == thread->GetName(); });

		    if (match == ms_GlobalsRegistration.end())
		    {
			    return true;
		    }

		    GlobalRegistration searchedGlobal = *match;

		    auto program                      = Memory::ScriptThreadToProgram(thread);
		    if (program->m_CodeBlocks)
		    {
			    Handle handle = Memory::FindScriptPattern(searchedGlobal.m_Pattern, program);

			    if (!handle.IsValid())
			    {
				    LOG("Failed to find global \"" << searchedGlobal.m_Name << "\"");
			    }
			    else
			    {
				    int globalIndex;

				    if (searchedGlobal.m_PatternIdiom == GlobalPatternIdiom::GLOBAL_U16)
				    {
					    globalIndex = handle.At(searchedGlobal.m_Offset).Value<uint16_t>() & 0xFFFFFF;
				    }
				    else
				    {
					    globalIndex = handle.At(searchedGlobal.m_Offset).Value<uint32_t>() & 0xFFFFFF;
				    }

				    Globals::SetGlobal(searchedGlobal, globalIndex);

				    LOG("Found global \"" << searchedGlobal.m_Name << "\" (Global: " << globalIndex << ")");
			    }
		    }

		    ms_GlobalsRegistration.erase(match);

		    return true;
	    });
}

bool Globals::Searching(int handle)
{
	return std::find_if(ms_GlobalsRegistration.begin(), ms_GlobalsRegistration.end(),
	                    [&](GlobalRegistration &reg) { return reg.m_Handle == handle; })
	    != ms_GlobalsRegistration.end();
}

int Globals::RegisterGlobal(std::string name, std::string scriptName, std::string pattern,
                                                     int patternOffset, GlobalPatternIdiom patternIdiom)
{
	if (std::find_if(ms_GlobalsRegistration.begin(), ms_GlobalsRegistration.end(),
	                 [&](GlobalRegistration &reg) { return reg.m_Name == name; })
	        != ms_GlobalsRegistration.end()
	    || Globals::GlobalFoundName(name))
	{
		return -1;
	}

	int handle = GetTickCount(); // Allows the handle to be truly unique

	ms_GlobalsRegistration.push_back({ name, scriptName, pattern, patternOffset, patternIdiom, handle });

	return handle;
}