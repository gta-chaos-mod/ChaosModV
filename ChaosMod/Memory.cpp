#include "stdafx.h"
#include "Memory.h"

Memory::Memory()
{
	MODULEINFO moduleInfo = { 0 };
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &moduleInfo, sizeof(moduleInfo));

	m_base = reinterpret_cast<DWORD64>(moduleInfo.lpBaseOfDll);
	m_end = reinterpret_cast<DWORD64>(moduleInfo.lpBaseOfDll) + moduleInfo.SizeOfImage;

	MH_Initialize();
}

Memory::~Memory()
{
	MH_Uninitialize();
}

DWORD64 Memory::GetBaseAddr()
{
	return m_base;
}

DWORD64 Memory::GetEndAddr()
{
	return m_end;
}

DWORD64 Memory::FindPattern(const char* pattern, const char* mask)
{
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

std::vector<Hash> Memory::GetAllWeapons()
{
	static WORD* something1; // dword_7FF6D9EF9748
	static DWORD64* something2; // qword_7FF6D9EF9740

	static std::vector<Hash> weapons;

	if (weapons.empty())
	{
		DWORD64 addr;
		// Thanks sigmaker for these memes
		addr = FindPattern("\x0F\xB7\x15\x00\x00\x00\x00\x45\x33\xD2\xFF\xCA\x78\x29\x4C\x8B\x1D\x00\x00\x00\x00\x46\x8D\x04\x12\x41\xD1\xF8\x4B\x8B\x0C\xC3\x44\x3B\x49\x10\x74\x13\x73\x06\x41\x8D\x50\xFF\xEB\x04\x45\x8D\x50\x01\x44\x3B\xD2\x7E\xDE\x33\xC9\x48\x85\xC9\x74\x04\x8B\x41\x1C", "xxx????xxxxxxxxxx????xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		something1 = (WORD*)(addr + 7 + *(DWORD*)(addr + 3));
		addr = FindPattern("\x4C\x8B\x1D\x00\x00\x00\x00\x46\x8D\x04\x12\x41\xD1\xF8\x4B\x8B\x0C\xC3\x44\x3B\x49\x10\x74\x13\x73\x06\x41\x8D\x50\xFF\xEB\x04\x45\x8D\x50\x01\x44\x3B\xD2\x7E\xDE\x33\xC9\x48\x85\xC9\x74\x04\x8B\x41\x1C", "xxx????xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
		something2 = (DWORD64*)(addr + 7 + *(DWORD*)(addr + 3));

		int v3;
		DWORD64 v4;
		DWORD64 v5;

		for (v3 = *something1 - 1; v3 >= 0; v3 = v4 - 1)
		{
			v4 = (DWORD)(v3);

			v5 = *((DWORD64*)*something2 + v4);

			Hash weapon = *(Hash*)(v5 + 16);

			// Filter out the weird invalid weapons
			if (GET_WEAPONTYPE_MODEL(weapon))
			{
				weapons.push_back(weapon);
			}
		}
	}

	return weapons;
}