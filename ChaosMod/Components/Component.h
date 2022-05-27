#pragma once

#include <memory>
#include <vector>

class Component;

inline std::vector<Component *> g_rgComponents;

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
		g_rgComponents.push_back(this);
	}

	virtual ~Component()
	{
		g_rgComponents.erase(std::find(g_rgComponents.begin(), g_rgComponents.end(), this));
	}

  public:
	Component(const Component &) = delete;

	Component &operator=(const Component &) = delete;

	virtual void OnModPauseCleanup()
	{
	}
	virtual void OnRun() = 0;

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};