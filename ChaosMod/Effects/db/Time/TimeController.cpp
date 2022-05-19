#include <stdafx.h>

static void OnStartMorning()
{
	SET_CLOCK_TIME(8, 0, 0);
}

// clang-format off
static RegisterEffect registerEffect1(OnStartMorning, EffectInfo
	{
		.Name = "Set Time To Morning",
		.Id = "time_morning",
		.EffectGroupType = EEffectGroupType::TimeChange
	}
);
// clang-format on

static void OnStartDay()
{
	SET_CLOCK_TIME(12, 0, 0);
}

// clang-format off
static RegisterEffect registerEffect2(OnStartDay, EffectInfo
	{
		.Name = "Set Time To Daytime",
		.Id = "time_day",
		.EffectGroupType = EEffectGroupType::TimeChange
	}
);
// clang-format on

static void OnStartEvening()
{
	SET_CLOCK_TIME(18, 0, 0);
}

// clang-format off
static RegisterEffect registerEffect3(OnStartEvening, EffectInfo
	{
		.Name = "Set Time To Evening",
		.Id = "time_evening",
		.EffectGroupType = EEffectGroupType::TimeChange
	}
);
// clang-format on

static void OnStartNight()
{
	SET_CLOCK_TIME(0, 0, 0);
}

// clang-format off
static RegisterEffect registerEffect4(OnStartNight, EffectInfo
	{
		.Name = "Set Time To Night",
		.Id = "time_night",
		.EffectGroupType = EEffectGroupType::TimeChange
	}
);
// clang-format on

static void OnTickTimelapse()
{
	ADD_TO_CLOCK_TIME(0, 1, 0);
}

// clang-format off
static RegisterEffect registerEffect6(nullptr, nullptr, OnTickTimelapse, EffectInfo
	{
		.Name = "Timelapse",
		.Id = "time_quickday",
		.IsTimed = true,
		.EffectGroupType = EEffectGroupType::TimeChange
	}
);