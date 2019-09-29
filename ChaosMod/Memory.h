#pragma once

class Memory
{
private:
	Memory(DWORD64 base, DWORD64 end);

public:
	static void Init();
	static void Stop();
	static DWORD64 GetBaseAddr();
	static DWORD64 GetEndAddr();
	static DWORD64 FindPattern(const char* pattern, const char* mask);

private:
	const DWORD64 m_base;
	const DWORD64 m_end;
};