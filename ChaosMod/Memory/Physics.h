#pragma once

#include "Memory/Entity.h"
#include "Memory/Handle.h"
#include "Memory/Memory.h"

#include "Util/Logging.h"

#include <scripthookv/inc/nativeCaller.h>
#include <scripthookv/inc/types.h>

#include <tuple>

using DWORD64 = unsigned long long;

namespace Memory
{
	inline bool DoesEntityHaveCollider(Entity entity)
	{
		static auto CEntity_GetColliderNonConst = []() -> void *(*)(DWORD64)
		{
			Handle handle = FindPattern("? 85 C0 74 ? ? 3B ? ? ? ? ? 75 ? ? 8B CF E8 ? ? ? ? ? 8D");
			if (handle.IsValid())
			{
				return handle.At(17).Into().Get<void *(DWORD64)>();
			}

			LOG("CEntity::GetColliderNonConst not found");
			return nullptr;
		}();

		auto handleAddr = GetScriptHandleBaseAddress(entity);
		return handleAddr && CEntity_GetColliderNonConst(handleAddr);
	}

	inline int GetNumFreeColliderSlots()
	{
		static auto phSimulator_sm_Instance = []
		{
			Handle handle = FindPattern("? 8B 0D ? ? ? ? ? 83 64 ? ? 00 ? 0F B7 D1 ? 33 C9 E8");
			if (handle.IsValid())
			{
				return handle.At(2).Into().Addr();
			}

			LOG("phSimulator::sm_Instance not found");
			return 0ull;
		}();

		static auto [usedCollidersOffset, maxCollidersOffset] = []
		{
			Handle handle = FindPattern("? 63 ? ? ? ? ? 3B ? ? ? ? ? 0F 8D ? ? ? ? ? 8B C8");
			if (handle.IsValid())
				return std::make_tuple(handle.At(3).Value<int>(), handle.At(9).Value<int>());

			LOG("usedCollidersOffset / maxCollidersOffset not found, using fallback values");
			return std::make_tuple(0x864, 0x860); // values in the latest version of the game.
		}();

		if (!phSimulator_sm_Instance)
			return 100;

		Handle handle(phSimulator_sm_Instance);
		handle = handle.Value<DWORD64>();

		if (handle.Addr() == 0)
			return 0;

		return (handle.At(maxCollidersOffset).Value<int>() - handle.At(usedCollidersOffset).Value<int>());
	}

	inline bool IsFreeToActivatePhysics()
	{
		const int MIN_FREE_COLLIDER_SLOTS = 50;

		return GetNumFreeColliderSlots() > MIN_FREE_COLLIDER_SLOTS;
	}

	// Safe version of APPLY_FORCE_TO_ENTITY with checks for available colliders to ensure the physics engine is not
	// overwhelmed.
	inline void ApplyForceToEntity(Entity entity, int forceFlags, float x, float y, float z, float offX, float offY,
	                               float offZ, int boneIndex, BOOL isDirectionRel, BOOL ignoreUpVec, BOOL isForceRel,
	                               BOOL p12, BOOL p13)
	{
		if (IsFreeToActivatePhysics() || DoesEntityHaveCollider(entity))
		{
			invoke<Void>(0xC5F68BE9613E2D18, entity, forceFlags, x, y, z, offX, offY, offZ, boneIndex, isDirectionRel,
			             ignoreUpVec, isForceRel, p12, p13);
		}
	}

	// Safe version of APPLY_FORCE_TO_ENTITY_CENTER_OF_MASS with checks for available colliders to ensure the physics
	// engine is not overwhelmed.
	inline void ApplyForceToEntityCenterOfMass(Entity entity, int forceType, float x, float y, float z, BOOL p5,
	                                           BOOL isDirectionRel, BOOL isForceRel, BOOL p8)
	{
		if (IsFreeToActivatePhysics() || DoesEntityHaveCollider(entity))
		{
			invoke<Void>(0x18FF00FC7EFF559E, entity, forceType, x, y, z, p5, isDirectionRel, isForceRel, p8);
		}
	}
}
