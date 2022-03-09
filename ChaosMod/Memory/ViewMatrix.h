#pragma once

#include "Memory.h"
#include "Handle.h"

#include "../Util/Natives.h"

namespace Memory
{
	inline Vector3 GetVector3(auto offset)
	{
		return Vector3(
			*reinterpret_cast<float*>(offset),
			*reinterpret_cast<float*>(offset + 0x4),
			*reinterpret_cast<float*>(offset + 0x8)
		);
	}

	inline void SetVector3(auto offset, Vector3 vec)
	{
		*reinterpret_cast<float*>(offset) = vec.x;
		*reinterpret_cast<float*>(offset + 0x4) = vec.y;
		*reinterpret_cast<float*>(offset + 0x8) = vec.z;
	}


	class ViewMatrix
	{
		uintptr_t address;

		Vector3 right;
		Vector3 forward;
		Vector3 up;
		Vector3 pos;

	public:

		ViewMatrix(uintptr_t address)
		{
			this->address = address;

			right = GetVector3(address + 0x00);
			forward = GetVector3(address + 0x10);
			up = GetVector3(address + 0x20);
			pos = GetVector3(address + 0x30);
		}

		ViewMatrix(Entity entity) : ViewMatrix((uintptr_t)(getScriptHandleBaseAddress(entity) + 0x60)) {}

		static ViewMatrix ViewMatrix2(Vehicle vehicle)
		{
			BYTE* offset = getScriptHandleBaseAddress(vehicle);
			return ViewMatrix(*reinterpret_cast<uintptr_t*>(vehicle + 0x30) + 0x20);
		}

		void setRightVector(Vector3 vec)
		{
			right = vec;
			SetVector3(address + 0x00, vec);
		}

		void setForwardVector(Vector3 vec)
		{
			forward = vec;
			SetVector3(address + 0x10, vec);
		}

		void setUpVector(Vector3 vec)
		{
			up = vec;
			SetVector3(address + 0x20, vec);
		}

		void setPosVector(Vector3 vec)
		{
			pos = vec;
			SetVector3(address + 0x30, vec);
		}

		Vector3 getRightVector()
		{
			right = GetVector3(address + 0x00);
			return right;
		}

		Vector3 getForwardVector()
		{
			forward = GetVector3(address + 0x10);
			return forward; 
		}

		Vector3 getUpVector()
		{
			up = GetVector3(address + 0x20);
			return up;
		}

		Vector3 getPosVector()
		{
			pos = GetVector3(address + 0x30);
			return pos;
		}

		ViewMatrix& operator*=(float mult)
		{
			setForwardVector(forward * mult);
			setRightVector(right * mult);
			setUpVector(up * mult);

			return *this;
		}
	};
}