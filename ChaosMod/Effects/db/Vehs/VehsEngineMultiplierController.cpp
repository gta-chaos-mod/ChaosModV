#include <stdafx.h>

static void OnStop()
{
	for (auto veh : GetAllVehs())
	{
		_SET_VEHICLE_ENGINE_POWER_MULTIPLIER(veh, 1.f);
		_SET_VEHICLE_ENGINE_TORQUE_MULTIPLIER(veh, 1.f);
	}
}

static void OnTickX2()
{
	for (auto veh : GetAllVehs())
	{
		_SET_VEHICLE_ENGINE_POWER_MULTIPLIER(veh, 2.f);
		_SET_VEHICLE_ENGINE_TORQUE_MULTIPLIER(veh, 2.f);
	}
}

static RegisterEffect registerEffect1(EFFECT_2XENGINE_VEHS, nullptr, OnStop, OnTickX2);

static void OnTickX10()
{
	for (auto veh : GetAllVehs())
	{
		_SET_VEHICLE_ENGINE_POWER_MULTIPLIER(veh, 10.f);
		_SET_VEHICLE_ENGINE_TORQUE_MULTIPLIER(veh, 10.f);
	}
}

static RegisterEffect registerEffect2(EFFECT_10XENGINE_VEHS, nullptr, OnStop, OnTickX10);

static void OnTickX05()
{
	for (auto veh : GetAllVehs())
	{
		_SET_VEHICLE_ENGINE_POWER_MULTIPLIER(veh, .5f);
		_SET_VEHICLE_ENGINE_TORQUE_MULTIPLIER(veh, .5f);
	}
}

static RegisterEffect registerEffect3(EFFECT_05XENGINE_VEHS, nullptr, OnStop, OnTickX05);