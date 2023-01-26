#pragma once

#include "Entity.h"
#include "Handle.h"
#include "Memory.h"

namespace Memory
{
	/* Thanks to SHVDN! */

	namespace EntityFragment
	{
		struct BoneIdAndIndexTuple
		{
			int boneId;                         // 0x00
			int boneIndex;                      // 0x04
			BoneIdAndIndexTuple *nextTupleAddr; // 0x08
		};

		struct CrSkeletonData
		{
			char pad1[0x10];                                   // 0x0
			BoneIdAndIndexTuple **boneIdAndIndexTupleArrayPtr; // 0x10
			unsigned short divisorForBoneIdAndIndexTuple;      // 0x18
			unsigned short unkValue;                           // 0x1A
			char pad2[0x42];                                   // 0x1C
			unsigned short boneCount;                          // 0x5E
		};

		struct FragDrawable
		{
			char pad[0x18];                 // 0x00
			CrSkeletonData *crSkeletonData; // 0x18
		};

		struct FragTypeChild
		{
			char pad[0x10];           // 0x00
			unsigned short boneIndex; // 0x10
			unsigned short boneId;    // 0x12
		};

		struct FragPhysicsLOD
		{
			char pad1[0xD0];                  // 0x00
			FragTypeChild **fragTypeChildArr; // 0xD0
			char pad2[0x46];                  // 0xD8
			BYTE fragmentGroupCount;          // 0x11E
		};

		struct FragPhysicsLODGroup
		{
			char pad[0x10];                             // 0x00
			FragPhysicsLOD *fragPhysicsLODAddresses[3]; // 0x10
		};

		struct GtaFragType
		{
			char pad1[0x30];                          // 0x00
			FragDrawable *fragDrawable;               // 0x30
			char pad2[0xB8];                          // 0x38
			FragPhysicsLODGroup *fragPhysicsLODGroup; // 0xF0
		};

		struct FragInst
		{
			char pad1[0x78];          // 0x00
			GtaFragType *gtaFragType; // 0x78
			char pad2[0x38];          // 0x80
			unsigned int unkType;     // 0xB8
		};

		inline bool (*detachFragmentPartByIndexFunc)(FragInst *, int);
		inline int getFragInstVFuncOffset;

		inline int GetBoneIndexByBoneId(CrSkeletonData *crSkeletonData, int boneId)
		{
			if (crSkeletonData->unkValue == 0)
			{
				if (boneId < crSkeletonData->boneCount)
					return boneId;

				return -1;
			}

			if (crSkeletonData->divisorForBoneIdAndIndexTuple == 0)
				return -1;

			BoneIdAndIndexTuple **firstTuplePtr = (crSkeletonData->boneIdAndIndexTupleArrayPtr
			                                       + (boneId % crSkeletonData->divisorForBoneIdAndIndexTuple));

			for (BoneIdAndIndexTuple *boneIdAndIndexTuple = *firstTuplePtr; boneIdAndIndexTuple != nullptr;
			     boneIdAndIndexTuple                      = boneIdAndIndexTuple->nextTupleAddr)
			{
				if (boneId == boneIdAndIndexTuple->boneId)
					return boneIdAndIndexTuple->boneIndex;
			}

			return -1;
		}

		inline CrSkeletonData *GetSkeletonData(FragInst *fragInst)
		{
			return fragInst->gtaFragType->fragDrawable->crSkeletonData;
		}

		inline FragPhysicsLOD *GetFragPhysicsLOD(FragInst *fragInst)
		{
			FragPhysicsLODGroup *fragPhysicsLODGroup = fragInst->gtaFragType->fragPhysicsLODGroup;
			auto *fragPhysicsLODAddresses            = fragPhysicsLODGroup->fragPhysicsLODAddresses;
			auto unkType                             = fragInst->unkType;

			if (unkType < 2)
			{
				return fragPhysicsLODAddresses[unkType];
			}
			else
			{
				return fragPhysicsLODAddresses[0];
			}
		}

		inline int GetFragmentGroupCount(FragInst *fragInst)
		{
			return GetFragPhysicsLOD(fragInst)->fragmentGroupCount;
		}

		inline int GetFragIndexByBoneIndex(FragInst *fragInst, int boneIndex)
		{
			int fragmentGroupCount           = GetFragmentGroupCount(fragInst);
			auto fragPhysicsLOD              = GetFragPhysicsLOD(fragInst);
			auto crSkeletonData              = GetSkeletonData(fragInst);

			FragTypeChild **fragTypeChildArr = fragPhysicsLOD->fragTypeChildArr;

			for (int i = 0; i < fragmentGroupCount; ++i)
			{
				unsigned short id = fragTypeChildArr[i]->boneId;

				if (boneIndex == GetBoneIndexByBoneId(crSkeletonData, id))
				{
					return i;
				}
			}

			return -1;
		}

		inline bool InitBones()
		{
			static const Handle handle =
			    Memory::FindPattern("0F BE 5E 06 48 8B CF FF 50 ?? 8B D3 48 8B C8 E8 ?? ?? ?? ?? 8B 4E ??");

			if (!handle.IsValid())
			{
				return false;
			}

			getFragInstVFuncOffset        = handle.At(9).Value<BYTE>();
			detachFragmentPartByIndexFunc = handle.At(15).Into().Get<bool(FragInst *, int)>();

			return true;
		}

		inline FragInst *GetFragInst(Entity entity)
		{
			if (!InitBones())
			{
				return nullptr;
			}

			DWORD64 address = GetScriptHandleBaseAddress(entity);
			return (*(FragInst * (__fastcall **)(DWORD64))(*reinterpret_cast<DWORD64 *>(address)
			                                               + getFragInstVFuncOffset))(address);
		}

	}

	inline int GetBoneIndexByFragIndex(Entity entity, int index)
	{
		EntityFragment::FragInst *fragInst = EntityFragment::GetFragInst(entity);
		if (!fragInst || index < 0 || index >= GetFragmentGroupCount(fragInst))
		{
			return -1;
		}

		auto fragPhysicsLOD                              = GetFragPhysicsLOD(fragInst);
		auto crSkeletonData                              = GetSkeletonData(fragInst);

		EntityFragment::FragTypeChild **fragTypeChildArr = fragPhysicsLOD->fragTypeChildArr;

		return GetBoneIndexByBoneId(crSkeletonData, fragTypeChildArr[index]->boneId);
	}

	inline int GetFragCount(Entity entity)
	{
		EntityFragment::FragInst *fragInst = EntityFragment::GetFragInst(entity);
		if (!fragInst)
		{
			return 0;
		}

		return GetFragmentGroupCount(fragInst);
	}

	inline void DetachBone(Entity entity, int boneIndex)
	{
		EntityFragment::FragInst *fragInst = EntityFragment::GetFragInst(entity);
		if (!fragInst)
		{
			return;
		}

		if (!IsFreeToActivatePhysics())
		{
			return;
		}

		int fragIndex = GetFragIndexByBoneIndex(fragInst, boneIndex);

		if (fragIndex == -1)
		{
			return;
		}

		if (fragIndex < GetFragmentGroupCount(fragInst))
		{
			EntityFragment::detachFragmentPartByIndexFunc(fragInst, fragIndex);
		}
	}
}