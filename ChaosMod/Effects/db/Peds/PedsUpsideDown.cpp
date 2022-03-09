/*
	Effect by Reguas
*/

#include <stdafx.h>
#include <map>

static std::map<Ped, Vector3> savedVectors;

static float ScalarProduct(Vector3 a, Vector3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

static void OnTick()
{
	for (Entity ped : GetAllPeds())
	{
		Memory::ViewMatrix vm = Memory::ViewMatrix(ped);

		if (!savedVectors.contains(ped))
		{
			savedVectors[ped] = vm.getUpVector();
		}
		if (ScalarProduct(savedVectors[ped], vm.getUpVector()) > 0) // angle between vectors < 90 deg
		{
			savedVectors[ped] = vm.getUpVector();
			vm.setUpVector(vm.getUpVector() * -1.f);
			vm.setRightVector(vm.getRightVector() * -1.f);
		}
	}
}

static void OnStop()
{
	savedVectors.clear();
}

static RegisterEffect registerEffect(EFFECT_PEDS_UPSIDEDOWN, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Upside Down Peds",
		.Id = "peds_upsidedown",
		.IsTimed = true
	}
);
