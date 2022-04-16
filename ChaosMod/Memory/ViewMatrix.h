#pragma once

#include "Memory.h"
#include "Handle.h"
#include "Entity.h"

#include "../Util/Natives.h"

namespace Memory
{
	class ViewMatrix
	{
		float* const matrix;

		// address must be valid or null
		ViewMatrix(uintptr_t address) : matrix(address ? reinterpret_cast<float*>(address) : nullptr) {}

		void SetVectorAt(int index, Vector3 vec)
		{
			matrix[index] = vec.x;
			matrix[index + 1] = vec.y;
			matrix[index + 2] = vec.z;
		}

		Vector3 VectorAt(int index) const
		{
			return Vector3(matrix[index], matrix[index + 1], matrix[index + 2]);
		}

		uintptr_t VMPtr(uintptr_t addr)
		{
			return addr ? addr + 0x60 : 0;
		}

	public:
		ViewMatrix() : ViewMatrix(0) {}

		ViewMatrix(Entity entity) : ViewMatrix(VMPtr(GetScriptHandleBaseAddress(entity))) {}

		static ViewMatrix ViewMatrix2(Vehicle vehicle)
		{
			auto addr = GetScriptHandleBaseAddress(vehicle);
			return addr ? ViewMatrix(*reinterpret_cast<uintptr_t*>(addr + 0x30) + 0x20) : ViewMatrix();
		}

		void SetRightVector(Vector3 vec)
		{
			SetVectorAt(0, vec);
		}

		void SetForwardVector(Vector3 vec)
		{
			SetVectorAt(4, vec);
		}

		void SetUpVector(Vector3 vec)
		{
			SetVectorAt(8, vec);
		}

		void SetPosVector(Vector3 vec)
		{
			SetVectorAt(12, vec);
		}


		Vector3 RightVector() const
		{
			return VectorAt(0);
		}

		Vector3 ForwardVector() const
		{
			return VectorAt(4);
		}

		Vector3 UpVector() const
		{
			return VectorAt(8);
		}

		Vector3 PosVector() const
		{
			return VectorAt(12);
		}

		void MultiplyDirectionVectors(float multiplier)
		{
			SetRightVector(RightVector() * multiplier);
			SetForwardVector(ForwardVector() * multiplier);
			SetUpVector(UpVector() * multiplier);
		}

		bool IsValid() const
		{
			return matrix != nullptr;
		}
	};
}
