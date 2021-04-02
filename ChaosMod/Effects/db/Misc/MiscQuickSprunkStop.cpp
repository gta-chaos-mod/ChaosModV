/*
    Effect by Last0xygen
*/

#include <stdafx.h>


static void OnStart()
{
    static Hash objectHash = GET_HASH_KEY("prop_vend_soda_02");
    Ped player = PLAYER_PED_ID();
    Vector3 pos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(player, 0, 5, 0);
    Object obj = CREATE_OBJECT(objectHash, pos.x, pos.y, pos.z, true, false, true);
    PLACE_OBJECT_ON_GROUND_PROPERLY(obj);
    SET_ENTITY_ROTATION(obj, GET_ENTITY_PITCH(player), -GET_ENTITY_ROLL(player), GET_ENTITY_HEADING(player), 0, true);
    SET_OBJECT_AS_NO_LONGER_NEEDED(&obj);
}

static RegisterEffect registerEffect(EFFECT_MISC_QUICK_SPRUNK_STOP, OnStart, nullptr, nullptr, EffectInfo
    {
        .Name = "Quick Sprunk Stop",
        .Id = "misc_quick_sprunk_stop",
        .EffectGroupType = EffectGroupType::SPAWN_GENERIC
    }
);
