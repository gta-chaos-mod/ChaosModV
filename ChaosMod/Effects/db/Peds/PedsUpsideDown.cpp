/*
	Effect by Reguas
*/

#include <stdafx.h>
#include <map>

#include "Memory/ViewMatrix.h"

static std::map<Ped, Vector3> savedVectors;

static float angleCos(Vector3 a, Vector3 b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z) / (a.Length() * b.Length());
}

static void OnTick()
{
	for (Ped ped : GetAllPeds())
	{
		Memory::ViewMatrix vm = Memory::ViewMatrix(ped);

		if (!vm.IsValid())
		{
			continue;
		}

		Vector3 uvInv = vm.UpVector() * -1.f;

		if (!savedVectors.contains(ped))
		{
			savedVectors[ped] = uvInv;
		}
		if (angleCos(savedVectors[ped], vm.UpVector()) < 0.99f) // angle between saved and actual vector > ~10 deg
		{
			savedVectors[ped] = uvInv;
			vm.SetUpVector(uvInv);
			vm.SetRightVector(vm.RightVector() * -1.f);
		}
	}
}

static void OnStop()
{
	for (Ped ped : GetAllPeds())
	{
		Memory::ViewMatrix vm = Memory::ViewMatrix(ped);

		if (!vm.IsValid())
		{
			continue;
		}

		if (savedVectors.contains(ped) && angleCos(savedVectors[ped], vm.UpVector()) > 0.99f)
		{
			vm.SetUpVector(vm.UpVector() * -1.f);
			vm.SetRightVector(vm.RightVector() * -1.f);
		}
	}

	savedVectors.clear();
}

//clang-format off
REGISTER_EFFECT(nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Upside Down Peds",
		.Id = "peds_upsidedown",
		.IsTimed = true
	}
);
