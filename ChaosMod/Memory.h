#pragma once
#include <vector>

typedef DWORD Hash;

class Memory
{
public:
	Memory();
	~Memory();

	DWORD64 GetBaseAddr();
	DWORD64 GetEndAddr();
	DWORD64 FindPattern(const char* pattern, const char* mask);
	std::vector<Hash> GetAllWeapons();
	void SetGravityLevel(float gravity);

private:
	DWORD64 m_base;
	DWORD64 m_end;
};