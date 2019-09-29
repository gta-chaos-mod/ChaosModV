#include "stdafx.h"
#include "Memory.h"

Memory* m_memory = nullptr;

Memory::Memory(DWORD64 base, DWORD64 end) : m_base(base), m_end(end)
{

}

void Memory::Init()
{
	if (!m_memory)
	{
		MODULEINFO moduleInfo = { 0 };
		GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &moduleInfo, sizeof(moduleInfo));

		m_memory = new Memory((DWORD64)moduleInfo.lpBaseOfDll, (DWORD64)moduleInfo.lpBaseOfDll + moduleInfo.SizeOfImage);
		MH_Initialize();
	}
}

void Memory::Stop()
{
	if (m_memory)
	{
		delete m_memory;
		m_memory = nullptr;

		MH_Uninitialize();
	}
}

DWORD64 Memory::GetBaseAddr()
{
	return m_memory ? m_memory->m_base : 0;
}

DWORD64 Memory::GetEndAddr()
{
	return m_memory ? m_memory->m_end : 0;
}

DWORD64 Memory::FindPattern(const char* pattern, const char* mask)
{
	if (!m_memory)
	{
		return 0;
	}

	int len = strlen(mask);
	int i = 0;
	for (DWORD64 addr = GetBaseAddr(); addr < GetEndAddr(); addr++)
	{
		if (mask[i] == '?' || *reinterpret_cast<char*>(addr) == pattern[i])
		{
			i++;
		}
		else
		{
			i = 0;
		}

		if (i == len)
		{
			return addr - i + 1;
		}
	}

	return 0;
}