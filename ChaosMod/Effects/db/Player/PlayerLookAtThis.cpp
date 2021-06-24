/*
    Effect by Gorakh
*/

#include <stdafx.h>

static Camera camera = 0;
static Ped facingPed = 0;

// Fallback coords to look at if there are no valid peds
static Vector3 fallbackCoords;

static void ActivateCamera()
{
    SET_CAM_ACTIVE(camera, true);
    RENDER_SCRIPT_CAMS(true, true, 500, 1, 1, 1);
}
static void DeactivateCamera()
{
    SET_CAM_ACTIVE(camera, false);
    RENDER_SCRIPT_CAMS(false, true, 500, 1, 1, 1);
}

static void UpdateCamera()
{
    ActivateCamera();
    
    // How much the new cameras Z should be offset, this is to avoid having your body block your view
    float cameraZOffset;

    // Here we attempt to estimate the camera root, i.e, what point the gameplay camera normally rotates around
    Vector3 estimatedCameraRoot;
    if (IS_PED_IN_ANY_VEHICLE(PLAYER_PED_ID(), false))
    {
        Vehicle playerVehicle = GET_VEHICLE_PED_IS_IN(PLAYER_PED_ID(), false);

        estimatedCameraRoot = GET_ENTITY_COORDS(playerVehicle, true);
        cameraZOffset = 2.f;
    }
    else
    {
        estimatedCameraRoot = GET_PED_BONE_COORDS(PLAYER_PED_ID(), 0x60f2, 0.f, 0.f, 0.f);
        cameraZOffset = 0.8f;
    }

    // The gameplay camera is not changed, so its current position is where the camera would be if the effect wasn't active
    Vector3 defaultCameraCoords = GET_GAMEPLAY_CAM_COORD();
    
    // How far away the default camera is from the estimated root
    float cameraDistance = estimatedCameraRoot.DistanceTo(defaultCameraCoords);

    if (facingPed != 0)
    {
        POINT_CAM_AT_ENTITY(camera, facingPed, 0.f, 0.f, 0.f, true);
    }
    else
    {
        POINT_CAM_AT_COORD(camera, fallbackCoords.x, fallbackCoords.y, fallbackCoords.z);
    }

    Vector3 cameraRotation = GET_CAM_ROT(camera, 2);

    Vector3 cameraPosition = Vector3::Init(estimatedCameraRoot.x, estimatedCameraRoot.y, estimatedCameraRoot.z);

    // Put the camera back a little bit, so it still has roughly the same distance from the player as the normal camera
    cameraPosition = cameraPosition + (cameraRotation.GetDirectionForRotation() * -cameraDistance);
    
    // Make the new camera Z roughly match the normal camera
    cameraPosition.z += cameraZOffset;

    CAM::SET_CAM_PARAMS(camera, cameraPosition.x, cameraPosition.y, cameraPosition.z, cameraRotation.x, cameraRotation.y, cameraRotation.z, GET_GAMEPLAY_CAM_FOV(), 0, 1, 1, 2);
}

static Vector3 GetRandomFallbackPoint()
{
    Ped player = PLAYER_PED_ID();

    float maxDistanceMultiplier = 1.f;
    float maxHeightMultiplier = 1.f;

    Vector3 playerCoords = GET_ENTITY_COORDS(player, !IS_ENTITY_DEAD(PLAYER_PED_ID(), true));

    Vector3 playerVelocity;
    if (IS_PED_IN_ANY_VEHICLE(player, false))
    {
        playerVelocity = GET_ENTITY_VELOCITY(GET_VEHICLE_PED_IS_IN(player, false));

        maxDistanceMultiplier = max(1.f, playerVelocity.Length());
        maxHeightMultiplier = max(1.f, playerVelocity.z);
    }
    else
    {
        playerVelocity = GET_ENTITY_VELOCITY(player);

        maxDistanceMultiplier = max(1.f, playerVelocity.Length());
        maxHeightMultiplier = max(1.f, playerVelocity.z);
    }

    playerCoords = playerCoords + (playerVelocity * 5.f);

    Vector3 fallback;
    fallback.x = playerCoords.x + (g_Random.GetRandomFloat(-40.f, 40.f) * maxDistanceMultiplier);
    fallback.y = playerCoords.y + (g_Random.GetRandomFloat(-40.f, 40.f) * maxDistanceMultiplier);
    fallback.z = playerCoords.z + (g_Random.GetRandomFloat(-5.f, 5.f) * maxHeightMultiplier);

    return fallback;
}
static bool CanFacePoint(Vector3 point)
{

    Ped player = PLAYER_PED_ID();

    float maxDistanceMultiplier = 1.f;
    float maxHeightMultiplier = 1.f;

    Vector3 playerCoords = GET_ENTITY_COORDS(player, !IS_ENTITY_DEAD(PLAYER_PED_ID(), true));

    Vector3 playerVelocity;
    if (IS_PED_IN_ANY_VEHICLE(player, false))
    {
        playerVelocity = GET_ENTITY_VELOCITY(GET_VEHICLE_PED_IS_IN(player, false));

        maxDistanceMultiplier = max(1.f, playerVelocity.Length());
        maxHeightMultiplier = max(1.f, playerVelocity.z);
    }
    else
    {
        playerVelocity = GET_ENTITY_VELOCITY(player);

        maxDistanceMultiplier = max(1.f, playerVelocity.Length());
        maxHeightMultiplier = max(1.f, playerVelocity.z);
    }

    playerCoords = playerCoords + (playerVelocity.GetDirectionForRotation() * playerVelocity.Length() * 5.f);

    if (point.DistanceTo(playerCoords) > 80.f * maxDistanceMultiplier)
        return false;

    return point.z - playerCoords.z < 7.5f * maxHeightMultiplier; // Stop the effect from focusing on points far above the player, as this will put the camera in the ground
}
static bool CanFacePed(Ped ped)
{
    return ped != 0 && DOES_ENTITY_EXIST(ped) && !IS_PED_DEAD_OR_DYING(ped, true) && !IS_PED_A_PLAYER(ped) && CanFacePoint(GET_ENTITY_COORDS(ped, true));
}

static void OnStart()
{
    camera = CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
    SET_CAM_AFFECTS_AIMING(camera, false);
}

static void OnTick()
{
    SET_CINEMATIC_MODE_ACTIVE(false);

    // If the player is in first person, switch to the next camera mode (First person messes up the effect for some reason)
    if (GET_FOLLOW_PED_CAM_VIEW_MODE() == 4)
        SET_FOLLOW_PED_CAM_VIEW_MODE(0);

    if (GET_FOLLOW_VEHICLE_CAM_VIEW_MODE() == 4)
        SET_FOLLOW_VEHICLE_CAM_VIEW_MODE(0);

    if (!CanFacePed(facingPed))
    {
        std::vector<Ped> peds = GetAllPeds();
        if (peds.size() > 0)
        {
            int iterations = 0;

            Ped selectedPed = 0;
            do
            {
                // If we have gone through twice the size of the list of peds, I think it's safe to assume there are no valid peds in the list
                if (iterations >= peds.size() * 2)
                    break;

                selectedPed = peds[g_Random.GetRandomInt(0, peds.size() - 1)];
                iterations++;
            } while (!CanFacePed(selectedPed));

            facingPed = selectedPed;
        }
    }

    if (!CanFacePed(facingPed))
    {
        // If no valid ped was found, focus on a random point instead
        facingPed = 0;

        if (!CanFacePoint(fallbackCoords))
            fallbackCoords = GetRandomFallbackPoint();
    }

    UpdateCamera();
}

static void OnStop()
{
    DeactivateCamera();
    DESTROY_CAM(camera, true);
    camera = 0;
}

static RegisterEffect registerEffect(EFFECT_PLAYER_LOOK_AT_THIS, OnStart, OnStop, OnTick, EffectInfo
    {
        .Name = "Look At This!",
        .Id = "player_look_at_this",
        .IsTimed = true,
        .IsShortDuration = true,
        .IncompatibleWith = { EFFECT_FLIP_CAMERA, EFFECT_PLAYER_GTA_2, EFFECT_PLAYER_QUAKE_FOV, EFFECT_PLAYER_BINOCULARS, EFFECT_PLAYER_ZOOMZOOM_CAM, EFFECT_FORCED_FP, EFFECT_FORCED_CINEMATIC, EFFECT_VEH_LOCKCAMERA }
    }
);