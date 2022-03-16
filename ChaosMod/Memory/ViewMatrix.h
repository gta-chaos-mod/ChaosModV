#pragma once

#include "Memory.h"
#include "Handle.h"

#include "../Util/Natives.h"

namespace Memory
{
	class ViewMatrix
	{
		float* matrix;

	public:

		ViewMatrix(uintptr_t address)
		{
			matrix = reinterpret_cast<float*>(address);
		}

		ViewMatrix(Entity entity) : ViewMatrix((uintptr_t)(getScriptHandleBaseAddress(entity) + 0x60)) {}

		static ViewMatrix ViewMatrix2(Vehicle vehicle)
		{
			BYTE* offset = getScriptHandleBaseAddress(vehicle);
			return ViewMatrix(*reinterpret_cast<uintptr_t*>((uintptr_t)vehicle + 0x30) + 0x20);
		}

		void setRightVector(Vector3 vec)
		{
			matrix[0] = vec.x;
			matrix[1] = vec.y;
			matrix[2] = vec.z;
		}

		void setForwardVector(Vector3 vec)
		{
			matrix[4] = vec.x;
			matrix[5] = vec.y;
			matrix[6] = vec.z;
		}

		void setUpVector(Vector3 vec)
		{
			matrix[8] = vec.x;
			matrix[9] = vec.y;
			matrix[10] = vec.z;
		}

		void setPosVector(Vector3 vec)
		{
			matrix[12] = vec.x;
			matrix[13] = vec.y;
			matrix[14] = vec.z;
		}

		Vector3 getRightVector()
		{
			return Vector3(matrix[0], matrix[1], matrix[2]);
		}

		Vector3 getForwardVector()
		{
			return Vector3(matrix[4], matrix[5], matrix[6]);
		}

		Vector3 getUpVector()
		{
			return Vector3(matrix[8], matrix[9], matrix[10]);
		}

		Vector3 getPosVector()
		{
			return Vector3(matrix[12], matrix[13], matrix[14]);
		}
	};
}