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
	EffectThread(RegisteredEffect* effect) : m_effect(effect), m_thread(CreateFiber(0, EffectThreadFunc, effect)), m_id(m_lastId++)
	{
		
	}

	bool operator==(DWORD64 threadId) const
	{
		return m_id == threadId;
	}

	bool operator==(void* thread) const
	{
		return m_thread == thread;
	}

public:
	DWORD PauseTime = 0;

	void Run() const
	{
		SwitchToFiber(m_thread);
	}

	DWORD64 GetId() const
	{
		return m_id;
	}

private:
	static inline DWORD64 m_lastId = 0;
	DWORD64 m_id = 0;

	void* m_thread;
	RegisteredEffect* m_effect;
};

class ThreadManager
{
public:
	ThreadManager();

public:
	DWORD64 CreateThread(RegisteredEffect* effect);
	void UnregisterThread(DWORD64 threadId);
	void PutThreadOnPause(DWORD ms);
	void RunThreads();
	void SwitchToMainThread();

private:
	std::list<EffectThread> m_threads;
	void* m_mainThread;
	DWORD64 m_lastTimestamp;
};

inline std::unique_ptr<ThreadManager> g_threadManager;