/*
    Effect by Last0xygen, modified
*/

#include <stdafx.h>

static void OnStart()
{
    static Hash objectHash = GET_HASH_KEY("prop_vend_soda_02");

    LoadModel(objectHash);

    Vector3 min, max;
    GET_MODEL_DIMENSIONS(objectHash, &min, &max);
    float height = (max - min).z;

    Entity player = PLAYER_PED_ID();

    Object lastVendingMachine = 0;
    for (int i = 0; i < g_MetaInfo.m_fChaosMultiplier; i++)
    {
        Vector3 pos;
        Entity relativeObject;
        if (lastVendingMachine)
        {
            // Not sure why the -1 is necessary here, but without it, the other vending machine will float above the previous one
            pos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(lastVendingMachine, 0.f, 0.f, height - 1.f);
            relativeObject = lastVendingMachine;
        }
        else
        {
            pos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(player, 0, 5, 0);
            relativeObject = player;
        }

        Object obj = CreatePoolProp(objectHash, pos.x, pos.y, pos.z, false);

        // Only do this for the first vending machine
        if (!lastVendingMachine)
        {
            PLACE_OBJECT_ON_GROUND_PROPERLY(obj);
        }

        SET_ENTITY_ROTATION(obj, GET_ENTITY_PITCH(relativeObject), -GET_ENTITY_ROLL(relativeObject), GET_ENTITY_HEADING(relativeObject), 0, true);

        lastVendingMachine = obj;
    }
}

static RegisterEffect registerEffect(EFFECT_MISC_QUICK_SPRUNK_STOP, OnStart, nullptr, nullptr, EffectInfo
    {
        .Name = "Quick Sprunk Stop",
        .Id = "misc_quick_sprunk_stop",
        .EEffectGroupType = EEffectGroupType::SpawnGeneric
    }
);
