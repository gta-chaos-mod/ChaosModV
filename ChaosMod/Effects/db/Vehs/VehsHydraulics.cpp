/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"
#include "Memory/Physics.h"

static const float m_fTargetNormalForce = 100.f;
static const float m_fTargetSideForce = 60.f;

//Not from TrainerV ;)
static void JumpVehicle(Entity vehicle, int forcetype, float x, float y, float z, float offX, float offY, float offZ, int boneindex, bool directionRel, bool ignoreUpVec, bool forceRel, bool p12 = false, bool p13 = true)
{
	if (!offX && !offY && !offZ) //If there is no offset, just apply it to the center. Why? Because even without an offset, it wouldn't not be perfectly in the center.
	{
		Memory::ApplyForceToEntityCenterOfMass(vehicle, forcetype, x, y, z, p12, directionRel, ignoreUpVec, p13);
		return;
	}
	Memory::ApplyForceToEntity(vehicle, forcetype, x, y, z, offX, offY, offZ, boneindex, directionRel, ignoreUpVec, forceRel, p12, p13);
}

static void OnTick()
{
	static const Ped plr = PLAYER_PED_ID();
	static const Vehicle veh = GET_VEHICLE_PED_IS_IN(plr, false);

	if (!DOES_ENTITY_EXIST(veh))
	{
		ENABLE_CONTROL_ACTION(0, 27, true); //Phone
		return;
	}

	DISABLE_CONTROL_ACTION(0, 76, true); //Handbrake
	DISABLE_CONTROL_ACTION(0, 27, true); //Phone
	
	if (!IS_VEHICLE_ON_ALL_WHEELS(veh))
	{
		if (GetAsyncKeyState(0x20)) //Up
			JumpVehicle(veh, 0, 0.f, 0.f, m_fTargetNormalForce, 0.f, 0.f, 0.f, 0, true, true, true, false, true);
		else if (GetAsyncKeyState(0x25)) //Left
			JumpVehicle(veh, 0, 0.f, 0.f, m_fTargetSideForce, -0.3f, 0.f, 0.f, 0, true, true, true, false, true);
		else if (GetAsyncKeyState(0x27)) //Right
			JumpVehicle(veh, 0, 0.f, 0.f, m_fTargetSideForce, 0.3f, 0.f, 0.f, 0, true, true, true, false, true);
		else if (GetAsyncKeyState(0x26)) //Front
			JumpVehicle(veh, 0, 0.f, 0.f, m_fTargetNormalForce, 0.f, 0.4f, 0.f, 0, true, true, true, false, true);
		else if (GetAsyncKeyState(0x28)) //Back
			JumpVehicle(veh, 0, 0.f, 0.f, m_fTargetNormalForce, 0.f, -0.4f, 0.f, 0, true, true, true, false, true);
	}
}

static void OnStop()
{
	ENABLE_CONTROL_ACTION(0, 76, true);
	ENABLE_CONTROL_ACTION(0, 172, true);
}

static RegisterEffect reg(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Hydraulics (Arrow/Space)",
		.Id = "vehs_hydraulics",
		.IsTimed = true,
		.IsShortDuration = true
	}
);
