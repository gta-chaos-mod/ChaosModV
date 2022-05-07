/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"
#include "Util/Camera.h"

static float ms_fMaxDist = 100;
static bool busy = false;

static std::string ptfxDict = "scr_mp_controller";
static std::string ptfxName = "scr_mp_elec_dst";

bool GetAimedAtEntity(Entity* e)
{
	Vector3 coords, normal;
	BOOL hit;
	Entity out;
	int handle = Util::RayCastGameplayCam(ms_fMaxDist, &hit, &coords, &normal, &out);
	*e = out;
	return (bool)hit;
}

bool ControlJustPressed(int p0, int p1)
{
	if (IS_CONTROL_JUST_PRESSED(p0, p1) || IS_DISABLED_CONTROL_JUST_PRESSED(p0, p1))
	{
		return true;
	}
	return false;
}

bool ControlPressed(int p0, int p1)
{
	return IS_CONTROL_PRESSED(p0, p1) || IS_DISABLED_CONTROL_PRESSED(p0, p1);
}

static void OnStart()
{
	REQUEST_NAMED_PTFX_ASSET(ptfxDict.c_str());
	while (!HAS_NAMED_PTFX_ASSET_LOADED(ptfxDict.c_str()))
	{
		WAIT(0);
	}
}

static void OnTick()
{
	Ped player = PLAYER_PED_ID();
	Hash weaponHash;
	if (!GET_CURRENT_PED_WEAPON(player, &weaponHash, true) || GET_WEAPONTYPE_GROUP(weaponHash) == 0xD49321D4 || GET_WEAPONTYPE_GROUP(weaponHash) == 0xB7BBD827)
	{
		return;
	}
	if (weaponHash == 0x42BF8A85 || weaponHash == 0xB62D1F67)
	{
		return;
	}
	if (ControlPressed(0, 25))
	{
		SET_TIME_SCALE(0.2f);
		DISABLE_CONTROL_ACTION(0, 24, true);
		DISABLE_CONTROL_ACTION(2, 257, true);
		if (ControlJustPressed(0, 24) || ControlJustPressed(2, 257))
		{
			if (busy) return;
			Entity tmp = 0;
			if (GetAimedAtEntity(&tmp))
			{
				if (!IS_ENTITY_A_PED(tmp)) return;
				busy = true;
				Vector3 pos = GET_ENTITY_COORDS(tmp, true);
				CLEAR_PED_TASKS_IMMEDIATELY(tmp);
				FREEZE_ENTITY_POSITION(tmp, true);
				USE_PARTICLE_FX_ASSET(ptfxDict.c_str());
				SET_PARTICLE_FX_SHOOTOUT_BOAT(1);
				int ptfx = START_PARTICLE_FX_NON_LOOPED_AT_COORD(ptfxName.c_str(), pos.x, pos.y, pos.z, 0.f, 0.f, 0.f, 9.f, false, false, false);
				SHOOT_SINGLE_BULLET_BETWEEN_COORDS(pos.x, pos.y, pos.z+0.6f, pos.x,pos.y, pos.z, 1000, false, 0x3656C8C1, player, false, true, 9999);
				busy = false;
				FREEZE_ENTITY_POSITION(tmp, false);
			}
		}
		else busy = false;
	}
	else
	{
		SET_TIME_SCALE(1.f);
		busy = false;
	}
}

static void OnStop()
{
	SET_TIME_SCALE(1.f);
	busy = false;
}

static RegisterEffect reg(EFFECT_PLAYER_ZEUS, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Smite",
		.Id = "player_zeus",
		.IsTimed = true,
		.IsShortDuration = false
	}
);