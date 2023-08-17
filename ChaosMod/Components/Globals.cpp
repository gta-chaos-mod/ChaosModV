#include <stdafx.h>

#include "Globals.h"

#include "Lib/scrThread.h"

#include "Memory/Script.h"

std::map<std::string, int> Globals::m_GlobalsIndexMap = {};
static std::vector<GlobalRegistration> m_GlobalsRegistration = {};

Globals::Globals() : Component()
{
	REGISTER_GLOBAL("CellPhoneState", "dialogue_handler",
	                "2D 00 02 00 ? 52 ? ? 41 ? 72 08 2A 06 56 ? ? 52 ? ? 41 ? 71 08 20 56 ? ? 72", 6,
	                GlobalPatternIdiom::GLOBAL_U16);	
	
	REGISTER_GLOBAL("CellPhoneLock", "dialogue_handler",
	                "72 54 ? ? 5d ? ? ? 71 2c ? ? ? 2b 72 54 ? ? 77 54 ? ? 52 ? ? 25 ? 2c ? ? ? 53 ? ? 06 56 ? ? 52 ? ? 76", 2,
	                GlobalPatternIdiom::GLOBAL_U16);


	m_SearchGlobalsListener.Register(
	    Hooks::OnScriptThreadRun,
	    [&](rage::scrThread *thread)
	    {
			// Save a f**k-tonne of CPU resources by destructing the event listener when we no longer have anymore globals to find
			if (m_GlobalsRegistration.size() <= 0)
			{
			    m_SearchGlobalsListener.~ChaosEventListener();
			    return true;
			}

		    auto match =
		        std::find_if(m_GlobalsRegistration.begin(), m_GlobalsRegistration.end(),
		                     [&thread](GlobalRegistration &reg) { return reg.m_ScriptName == thread->GetName(); });

		    if (match == m_GlobalsRegistration.end())
			    return true;

		    GlobalRegistration searchedGlobal = *match;

		    auto program = Memory::ScriptThreadToProgram(thread);
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
					    globalIndex = handle.At(searchedGlobal.m_Offset).Value<uint16_t>() & 0xFFFFFF;
				    else
					    globalIndex = handle.At(searchedGlobal.m_Offset).Value<uint32_t>() & 0xFFFFFF;

				    Globals::SetGlobalIndex(searchedGlobal.m_Name, globalIndex);

				    LOG("Found global \"" << searchedGlobal.m_Name << "\" (Global: " << globalIndex << ")");
			    }
		    }

		    m_GlobalsRegistration.erase(match);

		    return true;
	    });
}