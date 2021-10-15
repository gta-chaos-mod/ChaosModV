
/*
	Effect by casperge
*/

#include <stdafx.h>

static void OnStart()
{
	
	STREAMING::REQUEST_IPL("plg_01");
	STREAMING::REQUEST_IPL("prologue01");
	STREAMING::REQUEST_IPL("prologue01_lod");
	STREAMING::REQUEST_IPL("prologue01c");
	STREAMING::REQUEST_IPL("prologue01c_lod");
	STREAMING::REQUEST_IPL("prologue01d");
	STREAMING::REQUEST_IPL("prologue01d_lod");
	STREAMING::REQUEST_IPL("prologue01e");
	STREAMING::REQUEST_IPL("prologue01e_lod");
	STREAMING::REQUEST_IPL("prologue01f");
	STREAMING::REQUEST_IPL("prologue01f_lod");
	STREAMING::REQUEST_IPL("prologue01g");
	STREAMING::REQUEST_IPL("prologue01h");
	STREAMING::REQUEST_IPL("prologue01h_lod");
	STREAMING::REQUEST_IPL("prologue01i");
	STREAMING::REQUEST_IPL("prologue01i_lod");
	STREAMING::REQUEST_IPL("prologue01j");
	STREAMING::REQUEST_IPL("prologue01j_lod");
	STREAMING::REQUEST_IPL("prologue01k");
	STREAMING::REQUEST_IPL("prologue01k_lod");
	STREAMING::REQUEST_IPL("prologue01z");
	STREAMING::REQUEST_IPL("prologue01z_lod");
	STREAMING::REQUEST_IPL("plg_02");
	STREAMING::REQUEST_IPL("prologue02");
	STREAMING::REQUEST_IPL("prologue02_lod");
	STREAMING::REQUEST_IPL("plg_03");
	STREAMING::REQUEST_IPL("prologue03");
	STREAMING::REQUEST_IPL("prologue03_lod");
	STREAMING::REQUEST_IPL("prologue03b");
	STREAMING::REQUEST_IPL("prologue03b_lod");
	STREAMING::REQUEST_IPL("prologue03_grv_dug");
	STREAMING::REQUEST_IPL("prologue03_grv_dug_lod");
	STREAMING::REQUEST_IPL("prologue_grv_torch");
	STREAMING::REQUEST_IPL("plg_04");
	STREAMING::REQUEST_IPL("prologue04");
	STREAMING::REQUEST_IPL("prologue04_lod");
	STREAMING::REQUEST_IPL("prologue04b");
	STREAMING::REQUEST_IPL("prologue04b_lod");
	STREAMING::REQUEST_IPL("prologue04_cover");
	STREAMING::REQUEST_IPL("des_protree_end");
	STREAMING::REQUEST_IPL("des_protree_start");
	STREAMING::REQUEST_IPL("des_protree_start_lod");
	STREAMING::REQUEST_IPL("plg_05");
	STREAMING::REQUEST_IPL("prologue05");
	STREAMING::REQUEST_IPL("prologue05_lod");
	STREAMING::REQUEST_IPL("prologue05b");
	STREAMING::REQUEST_IPL("prologue05b_lod");
	STREAMING::REQUEST_IPL("plg_06");
	STREAMING::REQUEST_IPL("prologue06");
	STREAMING::REQUEST_IPL("prologue06_lod");
	STREAMING::REQUEST_IPL("prologue06b");
	STREAMING::REQUEST_IPL("prologue06b_lod");
	STREAMING::REQUEST_IPL("prologue06_int");
	STREAMING::REQUEST_IPL("prologue06_int_lod");
	STREAMING::REQUEST_IPL("prologue06_pannel");
	STREAMING::REQUEST_IPL("prologue06_pannel_lod");
	STREAMING::REQUEST_IPL("prologue_m2_door");
	STREAMING::REQUEST_IPL("prologue_m2_door_lod");
	STREAMING::REQUEST_IPL("plg_occl_00");
	STREAMING::REQUEST_IPL("prologue_occl");
	STREAMING::REQUEST_IPL("plg_rd");
	STREAMING::REQUEST_IPL("prologuerd");
	STREAMING::REQUEST_IPL("prologuerdb");
	STREAMING::REQUEST_IPL("prologuerd_lod");
	

	Hash prologuecar = GET_HASH_KEY("rancherxl2");
	LoadModel(prologuecar);
	Vehicle veh = CREATE_VEHICLE(prologuecar, 4341.123f, -5081.173f, 110.934f, 82.441f, true, false, false);
	SET_VEHICLE_ENGINE_ON(veh, true, true, false);
	Ped player = PLAYER_PED_ID();
	SET_PED_INTO_VEHICLE(player, veh, -1);
	SET_VEHICLE_FORWARD_SPEED(veh, 20);

	static const Hash model = GET_HASH_KEY("ig_brad");

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_COMPANION_BRAD", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, GET_HASH_KEY("PLAYER"));
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, GET_HASH_KEY("PLAYER"), relationshipGroup);

	Ped playerPed = PLAYER_PED_ID();
	Vector3 playerPos = GET_ENTITY_COORDS(playerPed, false);

	Ped ped = CreatePoolPed(4, model, playerPos.x, playerPos.y, playerPos.z, GET_ENTITY_HEADING(playerPed));
	
	SET_PED_INTO_VEHICLE(ped, veh, -2);
	

	SET_PED_SUFFERS_CRITICAL_HITS(ped, false);

	SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
	SET_PED_HEARING_RANGE(ped, 9999.f);

	SET_PED_AS_GROUP_MEMBER(ped, GET_PLAYER_GROUP(PLAYER_ID()));

	GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_MICROSMG"), 9999, true, true);
	GIVE_WEAPON_TO_PED(ped, GET_HASH_KEY("WEAPON_RPG"), 9999, true, true);

	SET_PED_ACCURACY(ped, 100);
	SET_PED_FIRING_PATTERN(ped, 0xC6EE6B4C);
	
}



static RegisterEffect registerEffect(PLAYER_TELEPORT_NORTHYANKTON, OnStart, OnStop, OnTick, EffectInfo
	{
		.Name = "Beat the train",
		.Id = "player_teleport_yankton"
	}
);