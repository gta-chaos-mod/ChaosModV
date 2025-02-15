#include <stdafx.h>

#include "EffectThreads.h"

#include "Util/Script.h"

static std::unordered_map<LPVOID, std::unique_ptr<EffectThread>> m_Threads;

static auto _StopThreadImmediately(auto it)
{
	auto &[threadId, thread] = *it;

	// Give thread a chance to stop gracefully
	// OK so maybe not really immediately but it's still blocking
	thread->Stop();
	int count = 0;
	while (!thread->HasStopped() && count++ < 20)
	{
		SwitchToFiber(g_MainThread);
		thread->OnRun();
	}

	return m_Threads.erase(it);
}

namespace EffectThreads
{
	LPVOID CreateThread(RegisteredEffect *effect)
	{
		auto thread         = std::make_unique<EffectThread>(effect);
		auto threadId       = thread->Thread;
		m_Threads[threadId] = std::move(thread);
		DEBUG_LOG("Created Effect Thread " << threadId);

		return threadId;
	}

	void StopThread(LPVOID threadId)
	{
		if (m_Threads.contains(threadId))
			m_Threads.at(threadId)->Stop();
	}

	void StopThreadImmediately(LPVOID threadId)
	{
		if (m_Threads.contains(threadId))
			_StopThreadImmediately(m_Threads.find(threadId));
	}

	void StopThreads()
	{
		for (auto &[threadId, thread] : m_Threads)
			thread->Stop();
	}

	void StopThreadsImmediately()
	{
		for (auto it = m_Threads.begin(); it != m_Threads.end();)
			it = _StopThreadImmediately(it);
	}

	void PauseThisThread(DWORD timeMs)
	{
		auto fiber = GetCurrentFiber();
		if (m_Threads.contains(fiber))
			m_Threads.at(fiber)->PauseTimestamp = GetTickCount64() + timeMs;
	}

	bool IsThreadPaused(LPVOID threadId)
	{
		if (!m_Threads.contains(threadId))
			return true;

		return m_Threads.at(threadId)->PauseTimestamp > GetTickCount64();
	}

	void _RunThread(auto &it)
	{
		auto &[threadId, thread] = *it;

		if (thread->HasStopped())
		{
			it = m_Threads.erase(it);
			return;
		}

		if (GetTickCount64() >= thread->PauseTimestamp)
			thread->OnRun();

		it++;
	}

	void RunThreads()
	{
		for (auto it = m_Threads.begin(); it != m_Threads.end();)
			_RunThread(it);
	}

	void RunThread(LPVOID threadId)
	{
		auto result = m_Threads.find(threadId);
		if (result != m_Threads.end())
			_RunThread(result);
	}

	bool DoesThreadExist(LPVOID threadId)
	{
		return m_Threads.contains(threadId);
	}

	bool HasThreadOnStartExecuted(LPVOID threadId)
	{
		if (!m_Threads.contains(threadId))
			return true;

		return m_Threads.at(threadId)->HasOnStartExecuted();
	}

	bool IsThreadAnEffectThread()
	{
		return m_Threads.contains(GetCurrentFiber());
	}

	EffectThreadSharedData *GetThreadSharedData(LPVOID threadId)
	{
		if (!DoesThreadExist(threadId))
			return nullptr;

		return &m_Threads.at(threadId)->ThreadData.SharedData;
	}
}

namespace CurrentEffect
{
	static EffectThreadSharedData *GetCurrentThreadSharedData()
	{
		auto threadId = GetCurrentFiber();
		return EffectThreads::GetThreadSharedData(threadId);
	}

	float GetEffectCompletionPercentage()
	{
		auto sharedData = GetCurrentThreadSharedData();
		return !sharedData ? 0.f : sharedData->EffectCompletionPercentage;
	}

	void SetEffectSoundPlayOptions(const EffectSoundPlayOptions &soundPlayOptions)
	{
		auto sharedData = GetCurrentThreadSharedData();
		if (sharedData)
			sharedData->EffectSoundPlayOptions = soundPlayOptions;
	}

	void OverrideEffectName(const std::string &effectName)
	{
		auto sharedData = GetCurrentThreadSharedData();
		if (sharedData)
			sharedData->OverrideEffectName = effectName;
	}

	void OverrideEffectNameFromId(const std::string &effectId)
	{
		auto sharedData = GetCurrentThreadSharedData();
		if (sharedData)
			sharedData->OverrideEffectId = effectId;
	}
}