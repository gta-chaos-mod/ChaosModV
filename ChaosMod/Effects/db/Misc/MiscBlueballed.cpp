#include <stdafx.h>

static std::list<Object> blueBalls;

static void OnStart()
{
    blueBalls.clear();
}

static void onTickBalled()
{
    static DWORD64 lastTick = 0;
    DWORD64 curTick = GET_GAME_TIMER();
    int ballLimit = 15;
    Ped player = PLAYER_PED_ID();
    Vehicle playerVeh = GET_VEHICLE_PED_IS_IN(player, false);

    if (curTick > lastTick + 200)
    {
        Vector3 entityVel = GET_ENTITY_VELOCITY(player);
        Vector3 playerCoords = GET_ENTITY_COORDS(player, false);
        float predictionDistanceZ = 3;
        if (!IS_PED_IN_ANY_VEHICLE(player, false))
        {
            entityVel = GET_ENTITY_VELOCITY(player);
            playerCoords = GET_ENTITY_COORDS(player, false);
        }
        else
        {
            predictionDistanceZ += 3;
            entityVel = GET_ENTITY_VELOCITY(playerVeh);
            playerCoords = GET_ENTITY_COORDS(playerVeh, false);
        }
        lastTick = curTick;
        static Hash weaponHash = GET_HASH_KEY("weapon_specialcarbine");
        float predictionLimit = 6;
        float predictionSpeed = 0;
        float predictionDivider = 3;
        float absoluteVelY = std::abs(entityVel.y);
        float absoluteVelX = std::abs(entityVel.x);
        float magnitude = std::sqrt((entityVel.x * entityVel.x) + (entityVel.y * entityVel.y));
        if (magnitude < predictionLimit * predictionDivider && magnitude > -predictionLimit * predictionDivider)
        {
            predictionSpeed = magnitude / predictionDivider;
        }
        else
        {
            predictionSpeed = predictionLimit;
        }
        predictionSpeed = abs(predictionSpeed);
        float absoluteMagnitude = std::sqrt((absoluteVelX * absoluteVelX) + (absoluteVelY * absoluteVelY));
        Vector3 spawnCoords = playerCoords + entityVel / magnitude * predictionSpeed;
        float predictionVel = absoluteMagnitude + predictionDistanceZ - entityVel.z - predictionSpeed;
        spawnCoords.z = playerCoords.z + predictionDistanceZ;
        Object obj = CreatePoolProp(1441141378, spawnCoords.x, spawnCoords.y, spawnCoords.z, true);
        blueBalls.push_back(obj);
        Vector3 min, max;
        GET_MODEL_DIMENSIONS(1441141378, &min, &max);
        SET_OBJECT_PHYSICS_PARAMS(obj, 50, 1.f, 1.f, 0.f, 0.f, .5f, 0.f, 0.f, 0.f, 0.f, 0.f);
        int backupCounter = 10;
        while (backupCounter > 0 && GET_ENTITY_SPEED(obj) < 0.5f && DOES_ENTITY_EXIST(obj))
        {
            SHOOT_SINGLE_BULLET_BETWEEN_COORDS(spawnCoords.x, spawnCoords.y, spawnCoords.z + max.z - min.z, spawnCoords.x, spawnCoords.y, spawnCoords.z, 0, true, weaponHash, 0, false, true, 0.01);
            SET_ENTITY_VELOCITY(obj, 0.f, 0.f, -predictionVel);
            --backupCounter;
        }
    }

    for (std::list<Object>::iterator it2 = blueBalls.begin(); it2 != blueBalls.end(); )
    {
        Object currentBall = *it2;
        if (DOES_ENTITY_EXIST(currentBall))
        {
            it2++;
            continue;
        }
        it2 = blueBalls.erase(it2);
    }

    while (blueBalls.size() > ballLimit)
    {
        std::list<Object>::iterator it = blueBalls.begin();
        Object currentBall = *it;
        DELETE_OBJECT(&currentBall);
        it = blueBalls.erase(it);
    }
}

static RegisterEffect registerEffect(EFFECT_MISC_BLUEBALLED, OnStart, nullptr, onTickBalled, EffectInfo
    {
        .Name = "Get Blueballed",
        .Id = "misc_blueballed",
        .IsTimed = true,
        .EEffectGroupType = EEffectGroupType::SpawnGeneric
    }
);