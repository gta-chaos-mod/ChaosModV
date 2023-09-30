/*
    Effect by Bad Koa
*/

#include <stdafx.h>

enum DriveTasks
{
	BRAKE, //1
	BRAKE_REVERSE,//3
	LEFT_90_BRAKE, //4
	RIGHT_90_BRAKE, //5
	BRAKE_STRONG, //6
	LEFT_ACCEL, //7
	RIGHT_ACCEL, //8
	LEFT_RESTORE_WHELL, //10
	RIGTH_RESTORE_WHELL, //11
	LEFT_GO_REVERSE, //13
	RIGTH_GO_REVERSE, //14
	STRONGBRAKE_TURN, //19
	WEAKBRAKE_TURN_LEFTRIGHT, //20
	WEAKBRAKE_TURN_RIGHTLEFT, //21
	ACCEL_FAST, //23
	BRAKE_TURNLEFT_MORELEFT, //25
	BRAKE_TURNIGHT_MORERIGHT, //26
	BRAKE_TILLSTOP, //27
	BRAKE_REVERSEACCEL, //28
	BURNOUT, //30
	ACCEL_HANDBRAKE,//31
	ACCEL_HARD //32
};

struct DriveTaskData
{
	DriveTasks task;
	int taskValue;
};

static std::vector<DriveTaskData> validTasks = {
	{ LEFT_ACCEL, 7 },
	{ RIGHT_ACCEL, 8},
	{ LEFT_RESTORE_WHELL, 10},
	{ RIGTH_RESTORE_WHELL, 11},
	{ LEFT_GO_REVERSE, 13},
	{ RIGTH_GO_REVERSE, 14},
	{ WEAKBRAKE_TURN_LEFTRIGHT, 20},
	{ WEAKBRAKE_TURN_RIGHTLEFT, 21},
	{ ACCEL_FAST, 23},
	{ BRAKE_REVERSEACCEL, 28},
	{ BURNOUT, 30},
	{ ACCEL_HANDBRAKE, 31},
	{ ACCEL_HARD, 32}
};

static void OnTick()
{
	static DWORD64 lastTick = 0;
	DWORD64 curTick         = GET_GAME_TIMER();

	DWORD64 taskPeriod      = g_Random.GetRandomInt(1000,1500);

	// Check if the task period has passed
	if (curTick - lastTick >= taskPeriod)
	{	
		lastTick = curTick;

		for (Ped ped : GetAllPeds())
		{
			if (DOES_ENTITY_EXIST(ped) && !IS_PED_A_PLAYER(ped) && IS_PED_IN_ANY_VEHICLE(ped, false))
			{
				DriveTaskData task = validTasks.at(g_Random.GetRandomInt(0, validTasks.size() - 1));
				Vehicle veh        = GET_VEHICLE_PED_IS_IN(ped, false);
				TASK_VEHICLE_TEMP_ACTION(ped, veh, task.taskValue, taskPeriod);
			}
		}
	}
}

// clang-format off
REGISTER_EFFECT(nullptr, nullptr, OnTick, EffectInfo
{
    .Name = "Pepega Traffic",
    .Id = "pepega_traffic",
    .IsTimed = true,
	.IncompatibleWith = { "notraffic" }
}
);