#pragma once
#include <vector>

typedef DWORD Hash;

namespace Memory
{
	void Init();
	void Stop();

	DWORD64 FindPattern(const char* pattern, const char* mask);
	std::vector<Hash> GetAllWeapons();
	void SetGravityLevel(float gravity);
	void SetSnow(bool state);
	std::vector<Hash> GetAllVehModels();
};