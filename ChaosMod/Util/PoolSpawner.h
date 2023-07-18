#pragma once

using Ped     = int;
using Vehicle = int;
using Hash    = unsigned long;

void ClearEntityPool(int distance = 0);

Ped CreatePoolClonePed(Ped pedToClone);
Ped CreatePoolPed(int pedType, Hash modelHash, float x, float y, float z, float heading);
Ped CreateRandomPoolPed(float x, float y, float z, float heading);
Ped CreatePoolPedInsideVehicle(Vehicle vehicle, int pedType, Hash modelHash, int seatIdx);

Vehicle CreatePoolVehicle(Hash modelHash, float x, float y, float z, float heading);
Vehicle CreatePoolCloneVehicle(Vehicle vehToClone);

Object CreatePoolProp(Hash modelHash, float x, float y, float z, bool dynamic);
Object CreatePoolPropAttachedToPed(Hash modelHash, Ped ped, int boneIndex, float offsetX, float offsetY, float offsetZ,
                                   float rotX, float rotY, float rotZ, bool softPinning, bool collision, bool fixedRot);
