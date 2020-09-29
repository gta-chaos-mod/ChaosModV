#pragma once

#include "stdafx.h"


class EffectThread
{
public:
	void Loop();
	void Start(RegisteredEffect* effect, bool isTimed);
	void Stop();

	RegisteredEffect* currentEffect;
	bool isStopped = true;

private:
	bool shouldStop = false;
	bool isTimed = false;
};