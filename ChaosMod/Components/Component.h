#pragma once

#include <vector>

class Component;

inline std::vector<Component*> g_rgComponents;

class Component
{
public:
	Component()
	{
		g_rgComponents.push_back(this);
	}

	virtual ~Component()
	{
		g_rgComponents.erase(std::find(g_rgComponents.begin(), g_rgComponents.end(), this));
	}

	Component(const Component&) = delete;

	Component& operator=(const Component&) = delete;

	virtual void Run() = 0;
};