#include "stdafx.h"
#include "EffectThread.h"

void EffectThread::Loop()
{
	while (true)
	{
		WAIT(0);
		if (isStopped)
		{
			if (currentEffect != nullptr)
			{
				currentEffect->Start();
				if (isTimed)
				{
					isStopped = false;
				}
				else
				{
					currentEffect = nullptr;
				}
			}
		}
		else if (currentEffect)
		{
			if (shouldStop)
			{
				shouldStop = false;
				isStopped = true;
				currentEffect->Stop();
				currentEffect = nullptr;
			}
			else
			{
				currentEffect->Tick();
			}
		}
	}
}

void EffectThread::Start(RegisteredEffect* effect, bool timed)
{
	isTimed= timed;
	currentEffect = effect;
}

void EffectThread::Stop()
{
	shouldStop = true;
}
