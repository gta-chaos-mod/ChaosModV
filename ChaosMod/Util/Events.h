#pragma once

#include <functional>
#include <vector>

class _ChaosBaseEvent
{
};

template <typename CallbackT> class ChaosBaseEvent : public _ChaosBaseEvent
{
  public:
	struct Listener
	{
		CallbackT Callback;
		bool IsEventStillValid = true;

		bool operator==(const Listener &listener)
		{
			auto getFuncAddress = []<typename T, typename... U>(std::function<T(U...)> func)
			{
				auto **funcPtr = func.template target<T (*)(U...)>();
				return reinterpret_cast<std::uint64_t>(*funcPtr);
			};

			return getFuncAddress(Callback) == getFuncAddress(listener.Callback);
		}
	};

  protected:
	std::vector<Listener *> m_Listeners;

  public:
	using CallbackType = CallbackT;

	virtual ~ChaosBaseEvent()
	{
		for (auto &listener : m_Listeners)
		{
			listener->IsEventStillValid = false;
		}
	}

	void RegisterListener(Listener *listener)
	{
		m_Listeners.push_back(listener);
	}

	void RemoveListener(Listener *listener)
	{
		auto it = std::find(m_Listeners.begin(), m_Listeners.end(), listener);
		if (it != m_Listeners.end())
		{
			listener->IsEventStillValid = false;
			m_Listeners.erase(it);
		}
	}
};

template <typename... Args> class ChaosEvent : public ChaosBaseEvent<std::function<void(Args...)>>
{
  public:
	void Fire(Args... args)
	{
		for (const auto &listener : ChaosEvent::m_Listeners)
		{
			listener->Callback(args...);
		}
	}
};

template <typename... Args> class ChaosCancellableEvent : public ChaosBaseEvent<std::function<bool(Args...)>>
{
  public:
	bool Fire(Args... args)
	{
		bool result = true;
		for (const auto &listener : ChaosCancellableEvent::m_Listeners)
		{
			if (!listener->Callback(args...))
			{
				result = false;
			}
		}

		return result;
	}
};

template <class ChaosEventType>
requires std::is_base_of_v<_ChaosBaseEvent, ChaosEventType>
class ChaosEventListener
{
  private:
	struct EventData
	{
		ChaosEventType *Event;
		typename ChaosEventType::Listener Listener;
	};
	std::list<EventData> m_Events;

  public:
	~ChaosEventListener()
	{
		for (auto &event : m_Events)
		{
			if (event.Listener.IsEventStillValid)
			{
				event.Event->RemoveListener(&event.Listener);
			}
		}
	}

	void Register(ChaosEventType &event, typename ChaosEventType::CallbackType callback)
	{
		EventData eventData { .Event = &event, .Listener = { .Callback = callback } };
		m_Events.push_back(eventData);
		event.RegisterListener(&m_Events.back().Listener);
	}
};

#define CHAOS_EVENT_LISTENER(type) ChaosEventListener<decltype(type)>