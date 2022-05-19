#include <stdafx.h>

static void OnStartMorning()
{
	SET_CLOCK_TIME(8, 0, 0);
}

static RegisterEffect registerEffect(OnStartMorning, EffectInfo
	{
		.Name = "Set Time To Morning",
		.Id = "time_morning",
		.EffectGroupType = EEffectGroupType::TimeChange
	}
);

static void OnStartDay()
{
	SET_CLOCK_TIME(12, 0, 0);
}

static RegisterEffect registerEffect2(OnStartDay, EffectInfo
	{
		.Name = "Set Time To Daytime",
		.Id = "time_day",
		.EffectGroupType = EEffectGroupType::TimeChange
	}
);

static void OnStartEvening()
{
	SET_CLOCK_TIME(18, 0, 0);
}

static RegisterEffect registerEffect3(OnStartEvening, EffectInfo
	{
		.Name = "Set Time To Evening",
		.Id = "time_evening",
		.EffectGroupType = EEffectGroupType::TimeChange
	}
);

static void OnStartNight()
{
	SET_CLOCK_TIME(0, 0, 0);
}

static RegisterEffect registerEffect4(OnStartNight, EffectInfo
	{
		.Name = "Set Time To Night",
		.Id = "time_night",
		.EffectGroupType = EEffectGroupType::TimeChange
	}
);

static void OnTickTimelapse()
{
	ADD_TO_CLOCK_TIME(0, 1, 0);
}

static RegisterEffect registerEffect5(nullptr, nullptr, OnTickTimelapse, EffectInfo
	{
		.Name = "Timelapse",
		.Id = "time_quickday",
		.IsTimed = true,
		.EffectGroupType = EEffectGroupType::TimeChange
	}
);

static void OnStartSystem()
{
	int year, month, day, hour, minute, second;
	GET_LOCAL_TIME(&year, &month, &day, &hour, &minute, &second); // Can't pass nullptr or it will crash the game

	SET_CLOCK_TIME(hour, minute, second);
	SET_CLOCK_DATE(day, month, year);
}

static RegisterEffect registerEffect6(OnStartSystem, EffectInfo
	{
		.Name = "Local Time",
		.Id = "time_local_time",
		.EffectGroupType = EEffectGroupType::TimeChange
	}
);