#pragma once

using Ped     = int;
using Vehicle = int;
using Hash    = unsigned long;

void ClearEntityPool(int iDistance = 0);

Ped CreatePoolClonePed(Ped pedToClone);
Ped CreatePoolPed(int iPedType, Hash ulHodelHash, float fPosX, float fPosY, float fPosZ, float fHeading);
Ped CreateRandomPoolPed(float fPosX, float fPosY, float fPosZ, float fHeading);
Ped CreatePoolPedInsideVehicle(Vehicle vehicle, int iPedType, Hash ulModelHash, int iSeatIdx);

Vehicle CreatePoolVehicle(Hash ulModelHash, float fPosX, float fPosY, float fPosZ, float fHeading);
Vehicle CreatePoolCloneVehicle(Vehicle vehToClone);

Object CreatePoolProp(Object ulModelHash, float fPosX, float fPosY, float fPosZ, bool bDynamic);