#include <stdafx.h>

#include "Memory/Hooks/Hook.h"
#include "Util/Types.h"

#define PI 3.14159265

static enum AudioPositionHookType
{
	NONE,
	ANGLE,
	FIXEDPOS
} ms_eAudioPositionType;

static bool ms_bEnabledHook = false;
static float ms_fTargetAngle;
static float ms_eTargetEntity;
static Vector3 ms_vTargetPosition;

__int64(*_OG_rage__audRequestedSettings__SetPosition)(__int64 _this, Vec3V *vPosition);
__int64 _HK_rage__audRequestedSettings__SetPostition(__int64 _this, Vec3V *position)
{
	Vec3V* vOveride;

	switch (ms_eAudioPositionType)
	{	
	case AudioPositionHookType::FIXEDPOS:
		vOveride = ms_vTargetPosition.GetInternal();
		break;
	case AudioPositionHookType::ANGLE:
	{
		if (DOES_ENTITY_EXIST(ms_eTargetEntity))
		{
			Vector3 dir = GET_ENTITY_COORDS(ms_eTargetEntity, false) - Vector3::FromInternal(position);
			float reqAngle = atan2(dir.x, dir.y) / PI;
			vOveride = GetCoordAround(ms_eTargetEntity, reqAngle + ms_fTargetAngle, Vector3::FromInternal(position).DistanceTo(GET_ENTITY_COORDS(ms_eTargetEntity, false)) / 2, 0, true).GetInternal();
			break;
		}
	}
	case AudioPositionHookType::NONE: default:
		vOveride = position;
		break;
	}

	return _OG_rage__audRequestedSettings__SetPosition(_this, ms_bEnabledHook ? vOveride : position);
}

static bool OnHook()
{
	Handle handle = Memory::FindPattern("E8 ? ? ? ? 49 8B 8F ? ? ? ? 0F B6 81 ? ? ? ? 3D ? ? ? ? 74 26");
	if (!handle.IsValid())
	{
		return false;
	}

	Memory::AddHook(handle.Into().Get<void>(), _HK_rage__audRequestedSettings__SetPostition, &_OG_rage__audRequestedSettings__SetPosition);

	return true;
}

//Late hook since it relies on natives to calculate. If it is getting an angle offset from an entity 
static RegisterHook registerHook(OnHook, "rage__audRequestedSettings__SetPosition", true);

namespace Hooks
{
	void SetAudioAngleFromEntity(Entity entity, float angle)
	{
		ms_eTargetEntity = entity;
		ms_fTargetAngle = angle;
		ms_eAudioPositionType = ANGLE;
		ms_bEnabledHook = true;
	}

	void SetAudioPositionFixed(Vector3 vPos)
	{
		ms_vTargetPosition = vPos;
		ms_eAudioPositionType = FIXEDPOS;
		ms_bEnabledHook = true;
	}	
	
	void ResetAudioPostionHook()
	{
		ms_bEnabledHook = false;
		ms_eAudioPositionType = NONE;
	}
}