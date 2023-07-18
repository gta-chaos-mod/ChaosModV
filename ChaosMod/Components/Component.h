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
		void operator=(auto &&args)
		{
			std::apply(
			    [&](auto &&...args)
			    {
				    m_Ptr.reset();
				    m_Ptr = std::unique_ptr<T, Deleter>(new T(args...));
			    },
			    args);
		}

		T *operator()() const
		{
			return m_Ptr.get();
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

template <class T>
requires std::is_base_of_v<Component, T>
inline void InitComponent(auto &&...args)
{
	// For whatever reason the compiler prepends an additional template param to Args, breaking std::forward
	// We're just going to perfect forward using a tuple instead
	ComponentHolder<T>::Instance = std::forward_as_tuple(args...);
}

class Component
{
  protected:
	Component()
	{
		g_Components.insert(this);
	}

	virtual ~Component()
	{
		g_Components.erase(this);
	}

  public:
	Component(const Component &)            = delete;

	Component &operator=(const Component &) = delete;

	virtual void OnModPauseCleanup()
	{
	}

	virtual void OnRun()
	{
	}

	virtual void OnKeyInput(DWORD key, bool repeated, bool isUpNow, bool isCtrlPressed, bool isShiftPressed,
	                        bool isAltPressed)
	{
	}

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};