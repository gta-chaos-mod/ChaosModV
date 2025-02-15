#pragma once

#include <memory>
#include <set>

using DWORD = unsigned long;

class Component;

inline std::set<Component *> g_Components;

template <class T>
requires std::is_base_of_v<Component, T>
struct ComponentHolder
{
	class Ptr
	{
		struct Deleter
		{
			void operator()(T *ptr)
			{
				delete ptr;
			}
		};
		std::unique_ptr<T, Deleter> m_Ptr;

	  public:
		T *operator()() const
		{
			return m_Ptr.get();
		}

		void operator=(T *ptr)
		{
			m_Ptr.reset();
			m_Ptr = std::unique_ptr<T, Deleter>(ptr);
		}

		void Reset()
		{
			m_Ptr.reset();
		}
	};

	static inline Ptr Instance;
};

template <class T>
requires std::is_base_of_v<Component, T>
inline T *GetComponent()
{
	return ComponentHolder<T>::Instance();
}

template <class T>
requires std::is_base_of_v<Component, T>
inline bool ComponentExists()
{
	return ComponentHolder<T>::Instance();
}

template <class T, class X = T>
requires std::is_base_of_v<Component, T> && std::is_base_of_v<T, X>
inline void InitComponent(auto &&...args)
{
	ComponentHolder<T>::Instance = new X(args...);
}

template <class T>
requires std::is_base_of_v<Component, T>
inline void UninitComponent()
{
	ComponentHolder<T>::Instance.Reset();
}

class Component
{
  public:
	Component()
	{
		g_Components.insert(this);
	}

	~Component()
	{
		g_Components.erase(this);
	}

	Component(const Component &)            = delete;

	Component &operator=(const Component &) = delete;

	enum PauseCleanupFlags
	{
		// Passed if called from another thread
		PauseCleanupFlags_UnsafeCleanup = (1 << 0)
	};
	virtual void OnModPauseCleanup(PauseCleanupFlags cleanupFlags = {})
	{
	}

	virtual void OnRun()
	{
	}

	virtual void OnKeyInput(DWORD key, bool repeated, bool isUpNow, bool isCtrlPressed, bool isShiftPressed,
	                        bool isAltPressed)
	{
	}
};