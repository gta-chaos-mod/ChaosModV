#pragma once

#include "Effects/EffectSoundPlayOptions.h"
#include "Effects/Register/RegisteredEffects.h"
#include "Util/CrashHandler.h"

#include <scripthookv/inc/types.h>

#include <memory>

using DWORD   = unsigned long;
using DWORD64 = unsigned long long;

using LPVOID  = void *;

struct EffectThreadSharedData
{
	float EffectCompletionPercentage = 0.f;
	EffectSoundPlayOptions EffectSoundPlayOptions;
	std::string OverrideEffectName;
	std::string OverrideEffectId;
};

struct EffectThreadData
{
	RegisteredEffect *Effect = nullptr;
	bool HasOnStartExecuted  = false;
	bool IsRunning           = true;
	bool HasStopped          = false;

	void *CallerFiber        = nullptr;

	EffectThreadSharedData SharedData;

	EffectThreadData(RegisteredEffect *effect) : Effect(effect)
	{
	}
};

namespace EffectThreads
{
	LPVOID CreateThread(RegisteredEffect *effect);

	void StopThread(LPVOID threadId);
	void StopThreadImmediately(LPVOID threadId);
	void StopThreads();
	void StopThreadsImmediately();

	void PauseThisThread(DWORD timeMs);
	bool IsThreadPaused(LPVOID threadId);

	void RunThreads();
	void RunThread(LPVOID threadId);

	bool DoesThreadExist(LPVOID threadId);
	bool HasThreadOnStartExecuted(LPVOID threadId);

	bool IsThreadAnEffectThread();

	EffectThreadSharedData *GetThreadSharedData(LPVOID threadId);
};

inline void EffectThreadFunc(LPVOID data)
{
	SetUnhandledExceptionFilter(CrashHandler);

	auto &threadData = *reinterpret_cast<EffectThreadData *>(data);

	threadData.Effect->Start();
	threadData.HasOnStartExecuted = true;

	while (threadData.IsRunning)
	{
		SwitchToFiber(threadData.CallerFiber);
		threadData.Effect->Tick();
	}

	threadData.Effect->Stop();

	threadData.HasStopped = true;
	SwitchToFiber(threadData.CallerFiber);
}

class EffectThread
{
  public:
	DWORD64 PauseTimestamp = 0;
	// NOTE: A previous fiber handle can be reused for a new one
	// Do not assume they are uniquely identifiable for anything other than currently running fibers!
	LPVOID Thread          = nullptr;
	EffectThreadData ThreadData;

	EffectThread(RegisteredEffect *effect) : ThreadData(effect), Thread(CreateFiber(0, EffectThreadFunc, &ThreadData))
	{
	}

	~EffectThread()
	{
		DeleteFiber(Thread);
	}

	EffectThread(const EffectThread &)            = delete;

	EffectThread &operator=(const EffectThread &) = delete;

	friend bool operator==(const std::unique_ptr<EffectThread> &thisThread, LPVOID thread)
	{
		return thisThread->Thread == thread;
	}

	inline void OnRun()
	{
		ThreadData.CallerFiber = GetCurrentFiber();
		SwitchToFiber(Thread);
	}

	inline void Stop()
	{
		if (!ThreadData.HasStopped)
		{
			ThreadData.IsRunning = false;

			OnRun();
		}
	}

	inline bool HasStopped() const
	{
		return ThreadData.HasStopped;
	}

	inline bool HasOnStartExecuted() const
	{
		return ThreadData.HasOnStartExecuted;
	}

	inline bool IsStopping() const
	{
		return !ThreadData.IsRunning;
	}
};

namespace CurrentEffect
{
	float GetEffectCompletionPercentage();
	void SetEffectSoundPlayOptions(const EffectSoundPlayOptions &soundPlayOptions);
	void OverrideEffectName(const std::string &effectName);
	void OverrideEffectNameFromId(const std::string &effectId);
}