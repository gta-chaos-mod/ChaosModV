/*
	Effect By OnlyRealNubs, made in celebration of my birthday, Last0xygen for pointing out improvements, MoneyWasted for audio
*/

#include "stdafx.h"

//props, peds, dicts and other vars
std::string ptfxDict = "core";
std::string ptfxName = "ent_dst_gen_gobstop";
std::string pedModel = "s_m_y_clown_01";
std::string cakeModel = "v_res_cakedome";
std::string audBank = "DLC_BATTLE/BTL_CLUB_DJ_CALLOUT_CROWD_CHEER";
std::string audDict = "dlc_btl_club_dj_callout_crowd_cheers_sounds";
std::string audName = "dj_crowd_cheer";

int soundId = -1;

static void LoadAssets()
{	
	//load audio
	REQUEST_SCRIPT_AUDIO_BANK(audBank.c_str(), 0, -1);

	//load models
	std::vector<std::string> modelsToLoad = {pedModel, cakeModel};
	for (int i = 0; i < modelsToLoad.size(); i++)
	{
		REQUEST_MODEL(GET_HASH_KEY(modelsToLoad[i].c_str()));
		while (!HAS_MODEL_LOADED(GET_HASH_KEY(modelsToLoad[i].c_str())))
		{
			WAIT(0);
		}
	}

	//load ptfx
	REQUEST_NAMED_PTFX_ASSET(ptfxDict.c_str());
	while (!HAS_NAMED_PTFX_ASSET_LOADED(ptfxDict.c_str()))
	{
		WAIT(0);
	}
}

static void UnloadAssets()
{
	//unload audio
	RELEASE_NAMED_SCRIPT_AUDIO_BANK(audBank.c_str());

	//unload models
	std::vector<std::string> modelsToLoad = {pedModel, cakeModel};
	for (int i = 0; i < modelsToLoad.size(); i++)
	{
		if (!IS_MODEL_VALID(GET_HASH_KEY(modelsToLoad[i].c_str()))) continue;
		SET_MODEL_AS_NO_LONGER_NEEDED(GET_HASH_KEY(modelsToLoad[i].c_str()));
	}

	//unload ptfx
	REMOVE_NAMED_PTFX_ASSET(ptfxDict.c_str());
}

static void OnStart()
{
	//Load assets
	LoadAssets();

	Ped playerPed = PLAYER_PED_ID();
	
	//Create Relationship
	static const Hash playerGroup = GET_HASH_KEY("PLAYER");

	Hash relationshipGroup;
	ADD_RELATIONSHIP_GROUP("_BRTHDAY_CLOWN", &relationshipGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, relationshipGroup, playerGroup);
	SET_RELATIONSHIP_BETWEEN_GROUPS(0, playerGroup, relationshipGroup);

	//Check if the player is in a vehicle
	Vehicle plrVeh = 0;
	plrVeh = GET_VEHICLE_PED_IS_USING(playerPed);

	//Get the players position
	Vector3 plrPos = { 0,0,0 };
	plrPos = GET_ENTITY_COORDS(playerPed, !IS_ENTITY_DEAD(playerPed, true));

	//Create the clown
	Ped newPed = CreatePoolPed(4, GET_HASH_KEY(pedModel.c_str()), plrPos.x, plrPos.y, plrPos.z, 0.f);
	SET_PED_RELATIONSHIP_GROUP_HASH(newPed, relationshipGroup);
	SET_PED_AS_GROUP_MEMBER(newPed, GET_PLAYER_GROUP(PLAYER_ID()));
	if (DOES_ENTITY_EXIST(plrVeh))
	{
		SET_PED_INTO_VEHICLE(newPed, plrVeh, -2);
	}

	//Give cake to player
	Object cake = CreatePoolProp(GET_HASH_KEY(cakeModel.c_str()), 0.f, 0.f, 0.f, false);
	ATTACH_ENTITY_TO_ENTITY(cake, playerPed, GET_PED_BONE_INDEX(playerPed, 57005), 0.f, 0.f, 0.f, 0.f, 80.f, 0.f, true, true, false, true, 1, true);

	//Start the ptfx at the player ped (head)
	USE_PARTICLE_FX_ASSET(ptfxDict.c_str());
	START_PARTICLE_FX_NON_LOOPED_ON_PED_BONE(ptfxName.c_str(), playerPed, 0.f, 0.f, 0.2f, 0.f, 0.f, 0.f, GET_PED_BONE_INDEX(playerPed, 0x796e), 1.5f, false, false, false);

	//Play sound
	PLAY_SOUND_FROM_ENTITY(soundId, audName.c_str(), playerPed, audDict.c_str(), false, 0);

	//Unload assets
	UnloadAssets();
}

static RegisterEffect registerEffect(EFFECT_PLAYER_BIRTHDAY, OnStart, EffectInfo
	{
		.Name = "Happy Birthday!",
		.Id = "player_birthday"
	}
);
