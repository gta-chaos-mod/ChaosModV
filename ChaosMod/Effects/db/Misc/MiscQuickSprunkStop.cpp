/*
    Effect by Last0xygen, modified
*/

#include <stdafx.h>

static Entity SpawnVendingMachine(Entity relativeTo)
{
    static Hash objectHash = GET_HASH_KEY("prop_vend_soda_02");

    Vector3 pos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(relativeTo, 0, 5, 0);
    Object obj = CreatePoolProp(objectHash, pos.x, pos.y, pos.z, false);
    PLACE_OBJECT_ON_GROUND_PROPERLY(obj);
    SET_ENTITY_ROTATION(obj, GET_ENTITY_PITCH(relativeTo), -GET_ENTITY_ROLL(relativeTo), GET_ENTITY_HEADING(relativeTo), 0, true);

    return obj;
}

static void OnStart()
{
    Entity relativeEntity = PLAYER_PED_ID();
    for (int i = 0; i < g_MetaInfo.m_fChaosMultiplier; i++)
    {
        relativeEntity = SpawnVendingMachine(relativeEntity);
    }
}

static RegisterEffect registerEffect(EFFECT_MISC_QUICK_SPRUNK_STOP, OnStart, nullptr, nullptr, EffectInfo
    {
        .Name = "Quick Sprunk Stop",
        .Id = "misc_quick_sprunk_stop",
        .EEffectGroupType = EEffectGroupType::SpawnGeneric
    }
);
