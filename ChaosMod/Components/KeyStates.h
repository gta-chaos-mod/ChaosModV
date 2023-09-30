#pragma once

#include "Components/Component.h"

#include <array>

using DWORD = unsigned long;
using BYTE  = unsigned char;

class KeyStates : public Component
{
  private:
	enum class KeyState
	{
		Released,
		JustPressed,
		Pressed
	};
	std::array<KeyState, 256> m_KeyStates;

  public:
	virtual void OnRun() override;

	bool IsKeyPressed(BYTE key) const;
	bool IsKeyJustPressed(BYTE key) const;

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};