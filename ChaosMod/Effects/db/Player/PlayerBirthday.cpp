/*
	Effect By OnlyRealNubs, made in celebration of my birthday
*/

#include "stdafx.h"

//props, peds, dicts and other vars
std::string ptfxDict = "core";
std::string ptfxName = "ent_dst_gen_gobstop";
std::string pedModel = "s_m_y_clown_01";
std::string cakeModel = "v_res_cakedome";
std::string audDict = "DLC_TG_Running_Back_Sounds";
std::string audName = "Cheers";

int soundId = 0;

static void LoadShit()
{
	//load models
	std::vector<std::string> modelsToLoad = {pedModel, cakeModel};
	for (int i = 0; i < modelsToLoad.size(); i++)
	{
		while (!HAS_MODEL_LOADED(GET_HASH_KEY(modelsToLoad[i].c_str())))
		{
			REQUEST_MODEL(GET_HASH_KEY(modelsToLoad[i].c_str()));
			WAIT(1);
		}
	}

	//load ptfx
	while (!HAS_NAMED_PTFX_ASSET_LOADED(ptfxDict.c_str()))
	{
		REQUEST_NAMED_PTFX_ASSET(ptfxDict.c_str());
		WAIT(1);
	}
}

static void UnloadShit()
{
	//unload models
	std::vector<std::string> modelsToLoad = {pedModel, cakeModel};
	for (int i = 0; i < modelsToLoad.size(); i++)
	{
		if (!IS_MODEL_VALID(GET_HASH_KEY(modelsToLoad[i].c_str()))) continue;
		while (HAS_MODEL_LOADED(GET_HASH_KEY(modelsToLoad[i].c_str())))
		{
			SET_MODEL_AS_NO_LONGER_NEEDED(GET_HASH_KEY(modelsToLoad[i].c_str()));
			WAIT(1);
		}
	}

	//unload ptfx
	while (HAS_NAMED_PTFX_ASSET_LOADED(ptfxDict.c_str()))
	{
		REMOVE_NAMED_PTFX_ASSET(ptfxDict.c_str());
		WAIT(1);
	}
}

static void OnStart()
{
	LoadShit();

	Player player = PLAYER_ID();
	Ped playerPed = PLAYER_PED_ID();
	Vehicle plrVeh = 0;
	Vector3 plrPos = { 0,0,0 };

	//Create Relationship
	static const Hash playerGroup = GET_HASH_KEY("PLAYER");
	static const Hash civGroup = GET_HASH_KEY("CIVMALE");
	static const Hash femCivGroup = GET_HASH_KEY("CIVFEMALE");

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_BRTHDAY_CLOWN", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, GET_HASH_KEY("PLAYER"));
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, GET_HASH_KEY("PLAYER"), relationshipGroup);

	//Check if the player is in a vehicle
	plrVeh = GET_VEHICLE_PED_IS_USING(playerPed);

	//Get the players position
	plrPos = GET_ENTITY_COORDS(playerPed, !IS_ENTITY_DEAD(playerPed, true));

	//Create the clown
	Ped newPed = CREATE_PED(4, GET_HASH_KEY(pedModel.c_str()), plrPos.x, plrPos.y, plrPos.z, 0.f, true, false);
	SET_PED_RELATIONSHIP_GROUP_HASH(newPed, relationshipGroup);
	SET_PED_AS_GROUP_MEMBER(newPed, GET_PLAYER_GROUP(PLAYER_ID()));
	if (DOES_ENTITY_EXIST(plrVeh))
	{
		SET_PED_INTO_VEHICLE(newPed, plrVeh, -2);
	}

	//Give cake to player
	Object cake = CREATE_OBJECT(GET_HASH_KEY(cakeModel.c_str()), 0.f, 0.f, 0.f, true, false, false);
	ATTACH_ENTITY_TO_ENTITY(cake, playerPed, GET_PED_BONE_INDEX(playerPed, 57005), 0.f, 0.f, 0.f, 0.f, 80.f, 0.f, true, true, false, true, 1, true);

	//Start the ptfx at the player ped (head)
	USE_PARTICLE_FX_ASSET(ptfxDict.c_str());
	START_PARTICLE_FX_NON_LOOPED_ON_PED_BONE(ptfxName.c_str(), playerPed, 0.f, 0.f, 0.2f, 0.f, 0.f, 0.f, GET_PED_BONE_INDEX(playerPed, 0x796e), 1.5f, false, false, false);

	UnloadShit();
}

static RegisterEffect registerEffect(EFFECT_PLAYER_BIRTHDAY, OnStart, EffectInfo
	{
		.Name = "Happy Birthday!",
		.Id = "player_birthday"
	}
);
