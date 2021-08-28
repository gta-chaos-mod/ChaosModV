/*
    Effect by Last0xygen, modified
*/

#include <stdafx.h>

static void onTickSprunk()
{
    static DWORD64 lastTick = 0;
    DWORD64 curTick = GET_GAME_TIMER();

    if (curTick > lastTick + 2000)
    {
        lastTick = curTick;
        Ped player = PLAYER_PED_ID();
        float offset = 5;
        float entityPitch = GET_ENTITY_PITCH(player);
        float entityHeading = GET_ENTITY_HEADING(player);

        if (IS_PED_IN_ANY_VEHICLE(player, false))
        {
            offset = 8;
            Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(player, false);
            Vector3 vector = GET_ENTITY_SPEED_VECTOR(playerVeh, true);
            if (vector.y < -1.f)
            {
                offset -= 16;
                entityPitch = -GET_ENTITY_PITCH(player);
                entityHeading = -GET_ENTITY_HEADING(player);
            }
        }
        static Hash objectHash = GET_HASH_KEY("prop_vend_soda_02");
        Vector3 pos = GET_OFFSET_FROM_ENTITY_IN_WORLD_COORDS(player, 0, offset, 0);
        Object obj = CreatePoolProp(objectHash, pos.x, pos.y, pos.z, false);
        PLACE_OBJECT_ON_GROUND_PROPERLY(obj);
        SET_ENTITY_ROTATION(obj, entityPitch, -GET_ENTITY_ROLL(player), entityHeading, 0, true);
    }
}

static RegisterEffect registerEffect(EFFECT_MISC_QUICK_SPRUNK_STOP, nullptr, nullptr, onTickSprunk, EffectInfo
    {
        .Name = "Quick Sprunk Stops",
        .Id = "misc_quick_sprunk_stop",
        .IsTimed = true,
        .IsShortDuration = false,
        .EEffectGroupType = EEffectGroupType::SpawnGeneric
    }
);
