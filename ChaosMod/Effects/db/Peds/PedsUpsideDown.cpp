/*
	Effect by Reguas
*/

#include <stdafx.h>
#include <map>

static std::map<Ped, Vector3> savedVectors;

static float angleCos(Vector3 a, Vector3 b)
{
	return (a.x * b.x + a.y * b.y + a.z * b.z) / (a.Length() * b.Length());
}

static void OnTick()
{
	for (Entity ped : GetAllPeds())
	{
		Memory::ViewMatrix vm = Memory::ViewMatrix(ped);

		Vector3 uvInv = vm.getUpVector() * -1.f;

		if (!savedVectors.contains(ped))
		{
			savedVectors[ped] = uvInv;
		}
		if (angleCos(savedVectors[ped], vm.getUpVector()) < 0.99f) // angle between saved and actual vector > ~10 deg
		{
			savedVectors[ped] = uvInv;
			vm.setUpVector(uvInv);
			vm.setRightVector(vm.getRightVector() * -1.f);
		}
	}
}

static void OnStop()
{
	for (Entity ped : GetAllPeds())
	{
		Memory::ViewMatrix vm = Memory::ViewMatrix(ped);

		if (savedVectors.contains(ped) && angleCos(savedVectors[ped], vm.getUpVector()) > 0.99f)
		{
			vm.setUpVector(vm.getUpVector() * -1.f);
			vm.setRightVector(vm.getRightVector() * -1.f);
		}
	}

	savedVectors.clear();
}

static RegisterEffect registerEffect(EFFECT_PEDS_UPSIDEDOWN, nullptr, OnStop, OnTick, EffectInfo
	{
		.Name = "Upside Down Peds",
		.Id = "peds_upsidedown",
		.IsTimed = true
	}
);
