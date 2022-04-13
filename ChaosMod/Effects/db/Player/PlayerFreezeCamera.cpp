/*
	Effect By OnlyRealNubs
*/

#include "stdafx.h"

Cam cam = 0;
Vector3 pos = { 0,0,0 };

static void OnTick()
{

}

static void OnStop()
{

}

static void OnStart()
{
	cam = CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", 1);
}