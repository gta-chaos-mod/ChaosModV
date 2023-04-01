#pragma once

#include <functional>
#include <vector>

template <typename... Args> class ChaosEvent
{
	std::vector<std::function<void(Args...)>> m_Listeners;

  public:
	void AddListener(std::function<void(Args...)> listener)
	{
		m_Listeners.push_back(listener);
	}

	void RemoveListener(std::function<void(Args...)> listener)
	{
		auto it = std::find(m_Listeners.begin(), m_Listeners.end(), listener);
		if (it != m_Listeners.end())
		{
			m_Listeners.erase(it);
		}
	}

	void Fire(Args... args)
	{
		for (const auto &listener : m_Listeners)
		{
			listener(args...);
		}
	}
};

template <typename... Args> class ChaosCancellableEvent
{
	std::vector<std::function<bool(Args...)>> m_Listeners;

  public:
	void AddListener(std::function<bool(Args...)> listener)
	{
		m_Listeners.push_back(listener);
	}

	void RemoveListener(std::function<void(Args...)> listener)
	{
		auto it = std::find(m_Listeners.begin(), m_Listeners.end(), listener);
		if (it != m_Listeners.end())
		{
			m_Listeners.erase(it);
		}
	}

	bool Fire(Args... args)
	{
		bool result = true;
		for (const auto &listener : m_Listeners)
		{
			if (!listener(args...))
			{
				result = false;
			}
		}

		return result;
	}
};

template <typename T> class ChaosEventListener
{
	T m_Listener;

  public:
	ChaosEventListener(auto &event, T listener)
	{
		m_Listener = listener;
		event.AddListener(listener);
	}

	~ChaosEventListener()
	{
		m_Listener.RemoveListener(m_Listener);
	}
};