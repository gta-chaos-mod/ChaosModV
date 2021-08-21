#include <stdafx.h>

static void OnStartMorning()
{
	SET_CLOCK_TIME(8, 0, 0);
}

static RegisterEffect registerEffect1(EFFECT_TIME_MORNING, OnStartMorning, EffectInfo
	{
		.Name = "Set Time To Morning",
		.Id = "time_morning",
		.EEffectGroupType = EEffectGroupType::TimeChange
	}
);
static void OnStartDay()
{
	SET_CLOCK_TIME(12, 0, 0);
}

static RegisterEffect registerEffect2(EFFECT_TIME_DAY, OnStartDay, EffectInfo
	{
		.Name = "Set Time To Daytime",
		.Id = "time_day",
		.EEffectGroupType = EEffectGroupType::TimeChange
	}
);
static void OnStartEvening()
{
	SET_CLOCK_TIME(18, 0, 0);
}

static RegisterEffect registerEffect3(EFFECT_TIME_EVENING, OnStartEvening, EffectInfo
	{
		.Name = "Set Time To Evening",
		.Id = "time_evening",
		.EEffectGroupType = EEffectGroupType::TimeChange
	}
);
static void OnStartNight()
{
	SET_CLOCK_TIME(0, 0, 0);
}

static RegisterEffect registerEffect4(EFFECT_TIME_NIGHT, OnStartNight, EffectInfo
	{
		.Name = "Set Time To Night",
		.Id = "time_night",
		.EEffectGroupType = EEffectGroupType::TimeChange
	}
);
static void OnTickTimelapse()
{
	ADD_TO_CLOCK_TIME(0, 1, 0);
}

static RegisterEffect registerEffect6(EFFECT_QUICK_TIME, nullptr, nullptr, OnTickTimelapse, EffectInfo
	{
		.Name = "Timelapse",
		.Id = "time_quickday",
		.IsTimed = true,
		.EEffectGroupType = EEffectGroupType::TimeChange
	}
);