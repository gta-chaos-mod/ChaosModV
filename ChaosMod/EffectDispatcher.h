#pragma once
#include <vector>

class Effects;

class EffectDispatcher
{
public:
	EffectDispatcher();
	~EffectDispatcher();

public:
	void Draw();
	void UpdateTimer();
	void UpdateDisplayedTexts();
	void DispatchRandomEffect();

private:
	float m_percentage;
	Effects* m_effects;

	class DisplayedEffect
	{
	public:
		DisplayedEffect(const char* name, int timer) : Name(name), IsActive(true), Timer(timer), ShouldDisplay(true) {}

	public:
		inline void Update()
		{
			Timer--;
			if (Timer < 0)
			{
				IsActive = false;
			}
			if (Timer < -15)
			{
				ShouldDisplay = false;
				Timer = -15;
			}
		}

	public:
		const char* Name;
		bool IsActive;
		int Timer;
		bool ShouldDisplay;
	};

	std::vector<DisplayedEffect> m_displayedEffects;
};