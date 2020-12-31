#pragma once

#include "Effects/Effect.h"

#include <list>
#include <memory>

typedef unsigned long DWORD;
typedef unsigned long long DWORD64;

static void EffectThreadFunc(void* effect)
{
	extern void WAIT(DWORD ms);

	while (true)
	{
		reinterpret_cast<RegisteredEffect*>(effect)->Tick();

		WAIT(0);
	}
}

struct EffectThread
{
public:
	EffectThread(RegisteredEffect* effect) : m_effect(effect), Thread(CreateFiber(0, EffectThreadFunc, effect)), Id(m_lastId++)
	{
		
	}

	~EffectThread()
	{
		DeleteFiber(Thread);
	}

	friend bool operator==(const std::unique_ptr<EffectThread>& thisThread, DWORD64 threadId)
	{
		return thisThread->Id == threadId;
	}

	friend bool operator==(const std::unique_ptr<EffectThread>& thisThread, void* thread)
	{
		return thisThread->Thread == thread;
	}

public:
	int PauseTime = 0;
	void* const Thread;
	const DWORD64 Id = 0;

	void Run() const
	{
		SwitchToFiber(Thread);
	}

private:
	static inline DWORD64 m_lastId = 0;

	RegisteredEffect* m_effect;
};

namespace ThreadManager
{
	DWORD64 CreateThread(RegisteredEffect* effect);
	void UnregisterThread(DWORD64 threadId);
	void ClearThreads();
	void PutThreadOnPause(DWORD ms);
	void RunThreads();
	void SwitchToMainThread();
};