#include <stdafx.h>

#include "Globals.h"

#include "Lib/scrThread.h"

#include "Memory/Script.h"

std::vector<GlobalRegistration> m_GlobalsRegistration = {};

Globals::Globals() : Component()
{
	m_SearchGlobalsListener.Register(
	    Hooks::OnScriptThreadRun,
	    [&](rage::scrThread *thread)
	    {
		    if (m_GlobalsRegistration.size() <= 0)
		    {
			    return true;
		    }

		    auto match =
		        std::find_if(m_GlobalsRegistration.begin(), m_GlobalsRegistration.end(),
		                     [&thread](GlobalRegistration &reg) { return reg.m_ScriptName == thread->GetName(); });

		    if (match == m_GlobalsRegistration.end())
			    return true;

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

				    Globals::SetGlobalIndex(searchedGlobal.m_Name, globalIndex);

				    LOG("Found global \"" << searchedGlobal.m_Name << "\" (Global: " << globalIndex << ")");
			    }
		    }

		    m_GlobalsRegistration.erase(match);

		    return true;
	    });
}

void Globals::RegisterGlobal(std::string name, std::string scriptName, std::string pattern, int patternOffset,
                             GlobalPatternIdiom patternIdiom)
{
	if (std::find_if(m_GlobalsRegistration.begin(), m_GlobalsRegistration.end(),
	                 [&](GlobalRegistration &reg) { return reg.m_Name == name; })
	        != m_GlobalsRegistration.end()
	    || Globals::GetGlobalAddr<void>(name) != nullptr)
	{
		return;
	}

	m_GlobalsRegistration.push_back({ name, scriptName, pattern, patternOffset, patternIdiom });
}