/*
	Effect by Gorakh
*/

#include <stdafx.h>

static constexpr int NUM_FAKE_BARS = 7;

static void OnStart()
{
	MetaModifiers::m_bDisplayFakeEffectBars = true;
	MetaModifiers::m_ucNumFakeEffectBars = NUM_FAKE_BARS;
	MetaModifiers::m_ucRealEffectBarIndex = g_Random.GetRandomInt(0, NUM_FAKE_BARS);

	if (!MetaModifiers::m_pFakeEffectBars)
	{
		MetaModifiers::m_pFakeEffectBars = new MetaModifiers::FakeEffectBarInfo[NUM_FAKE_BARS];
	}

	int maxTimerTime = GetComponent<EffectDispatcher>()->GetMaxTimerTime();
	for (size_t i = 0; i < NUM_FAKE_BARS; i++)
	{
		MetaModifiers::m_pFakeEffectBars[i] = { (WORD)g_Random.GetRandomInt(0, maxTimerTime), 0.f };
	}
}

static void OnStop()
{
	MetaModifiers::m_bDisplayFakeEffectBars = false;
	MetaModifiers::m_ucNumFakeEffectBars = 0;
	MetaModifiers::m_ucRealEffectBarIndex = 0;

	GetComponent<EffectDispatcher>()->ClearAllFakeEffects();
}

static RegisterEffect registerEffect(EFFECT_META_FAKE_CHAOS, OnStart, OnStop, nullptr, EffectInfo
	{
		.Name = "Fake Chaos",
		.Id = "meta_fake_chaos",
		.IsTimed = true,
		.ExecutionType = EEffectExecutionType::Meta
	}
);