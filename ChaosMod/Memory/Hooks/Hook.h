#pragma once

#include <string>

enum MH_STATUS : int;

namespace Memory
{
	bool AddHook(void* pTarget, void* pDetour, void* ppOrig = nullptr);
}

class RegisteredHook;
inline RegisteredHook* g_pRegisteredHooks = nullptr;

class RegisteredHook
{
public:
	RegisteredHook(bool(*hookFunc)(), const std::string& name) : m_hookFunc(hookFunc), m_name(name)
	{
		if (g_pRegisteredHooks)
		{
			m_pNext = g_pRegisteredHooks;
		}

		g_pRegisteredHooks = this;
	}

	inline bool RunHook()
	{
		return m_hookFunc();
	}

	inline const std::string& GetName() const
	{
		return m_name;
	}

	inline RegisteredHook* GetNext() const
	{
		return m_pNext;
	}

private:
	RegisteredHook* m_pNext = nullptr;
	const std::string m_name;
	bool(*m_hookFunc)();
};

class RegisterHook
{
public:
	RegisterHook(bool(*hookFunc)(), const std::string&& name) : m_registeredHook(hookFunc, name)
	{

	}

private:
	const RegisteredHook m_registeredHook;
};

template <typename T>
class Hook
{
public:
	Hook(uintptr_t addr, T func) : address(addr), fn(func) { }
	virtual ~Hook();
	virtual void remove() = 0;
	uintptr_t address;
	T fn;
};

template <typename T>
Hook<T>::~Hook()
{
}

template <typename T>
class CallHook : public Hook<T>
{
public:
	CallHook(uintptr_t addr, T func) : Hook<T>(addr, func) { }
	~CallHook();
	virtual void remove();
};

template <typename T>
void CallHook<T>::remove()
{
	*reinterpret_cast<int32_t*>(address + 1) = static_cast<int32_t>((intptr_t)fn - (intptr_t)address - 5);
}

template <typename T>
CallHook<T>::~CallHook()
{
	remove();
}

class HookManager
{
public:
	template <typename T>
	static inline CallHook<T>* SetCall(uintptr_t address, T target)
	{
		T orig = reinterpret_cast<T>(*reinterpret_cast<int*>(address + 1) + (address + 5));

		HMODULE hModule = GetModuleHandle(NULL);

		auto pFunc = AllocateFunctionStub((void*)(hModule), (void*)target, 0);

		*reinterpret_cast<BYTE*>(address) = 0xE8;

		*reinterpret_cast<int32_t*>(address + 1) = static_cast<int32_t>((intptr_t)pFunc - (intptr_t)address - 5);

		return new CallHook<T>(address, orig);
	}

	static void* AllocateFunctionStub(void* origin, void* function, int type);

	static LPVOID FindPrevFreeRegion(LPVOID pAddress, LPVOID pMinAddr, DWORD dwAllocationGranularity);
};