/*
	Effect by Reguas
*/

#include <stdafx.h>

static Vector3 GetVector3(auto offset)
{
	return Vector3(
		*reinterpret_cast<float*>(offset),
		*reinterpret_cast<float*>(offset + 0x4),
		*reinterpret_cast<float*>(offset + 0x8)
	);
}

static void SetVector3(auto offset, Vector3 vec)
{
	*reinterpret_cast<float*>(offset) = vec.x;
	*reinterpret_cast<float*>(offset + 0x4) = vec.y;
	*reinterpret_cast<float*>(offset + 0x8) = vec.z;
}

static void MultiplyMatrix(auto offset, float multiplier)
{
	auto address = offset + 0x60;											// a matrix for passengers
	auto address2 = *reinterpret_cast<uintptr_t*>(offset + 0x30) + 0x20;	// a matrix for vehicle
	Vector3 fv = GetVector3(address + 0x00);
	Vector3 rv = GetVector3(address + 0x10);
	Vector3 uv = GetVector3(address + 0x20);

	SetVector3(address + 0x00, fv * multiplier);
	SetVector3(address + 0x10, rv * multiplier);
	SetVector3(address + 0x20, uv * multiplier);
	SetVector3(address2 + 0x00, fv * multiplier);
	SetVector3(address2 + 0x10, rv * multiplier);
	SetVector3(address2 + 0x20, uv * multiplier);
}

static void OnTick()
{
	static DWORD64 lastTick = GET_GAME_TIMER();
	DWORD64 curTick = GET_GAME_TIMER();
	Ped player = PLAYER_PED_ID();
	if (IS_PED_IN_ANY_VEHICLE(player, 0))
	{
		Entity vehicle = GET_VEHICLE_PED_IS_IN(player, 0);

		if (IS_THIS_MODEL_A_CAR(GET_ENTITY_MODEL(vehicle))) // unfortunately, doesn't work with bikes, and has much less effect on planes
		{
			static Vehicle savedVehicle = 0;
			static float savedHeading = 0;
			static float savedDelta = 0;
			static float multiplier = 1.f;

			if (savedVehicle != vehicle)
			{
				multiplier = 1.f;
				savedHeading = 0;
				savedDelta = 0;
			}

			float heading = GET_ENTITY_HEADING(vehicle);
			if (savedHeading == 0 || curTick - lastTick > 500)
			{
				savedHeading = heading;
				lastTick = curTick;
			}

			float delta = heading - savedHeading;
			if (delta < -180.f)
			{
				delta += 360.f;
			}
			if (delta > 180.f)
			{
				delta -= 360.f;
			}

			if (savedDelta == 0 && savedHeading != 0)
			{
				savedDelta = delta;
			}
			if (delta * savedDelta < 0 && abs(delta) >= 7.5f)
			{
				savedDelta = delta;
				savedHeading = heading;
				multiplier += 0.01f;
			}

			static float savedLength;
			float length;
			Vector3 rv, fv, uv, pv;
			GET_ENTITY_MATRIX(vehicle, &rv, &fv, &uv, &pv);
			length = fv.Length() + rv.Length() + uv.Length();

			if (savedVehicle != vehicle)
			{
				savedLength = length;
				savedVehicle = vehicle;
			}
			if (length - savedLength < 0.1f)
			{
				MultiplyMatrix(getScriptHandleBaseAddress(vehicle), multiplier);
			}
		}
	}
}

static RegisterEffect registerEffect(EFFECT_WIGGLE_CHANGES_SIZE, nullptr, nullptr, OnTick, EffectInfo
	{
		.Name = "Wiggle-Wiggle",
		.Id = "veh_wiggle_changes_size",
		.IsTimed = true
	}
);
