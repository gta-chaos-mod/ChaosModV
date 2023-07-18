#include <stdafx.h>

#include "KeyStates.h"

void KeyStates::OnRun()
{
	for (BYTE i = 0; i < std::numeric_limits<BYTE>::max(); i++)
	{
		m_KeyStates[i] = GetAsyncKeyState(i)
		                   ? m_KeyStates[i] != KeyState::Released ? KeyState::Pressed : KeyState::JustPressed
		                   : KeyState::Released;
	}
}

bool KeyStates::IsKeyPressed(BYTE key) const
{
	return m_KeyStates[key] != KeyState::Released;
}

bool KeyStates::IsKeyJustPressed(BYTE key) const
{
	return m_KeyStates[key] == KeyState::JustPressed;
}