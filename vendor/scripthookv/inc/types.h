/*
	THIS FILE IS A PART OF GTA V SCRIPT HOOK SDK
				http://dev-c.com
			(C) Alexander Blade 2015
*/

#pragma once

#include <windows.h>
#include <cmath>

typedef DWORD Void;
typedef DWORD Any;
typedef DWORD uint;
typedef DWORD Hash;
typedef int Entity;
typedef int Player;
typedef int FireId;
typedef int Ped;
typedef int Vehicle;
typedef int Cam;
typedef int CarGenerator;
typedef int Group;
typedef int Train;
typedef int Pickup;
typedef int Object;
typedef int Weapon;
typedef int Interior;
typedef int Blip;
typedef int Texture;
typedef int TextureDict;
typedef int CoverPoint;
typedef int Camera;
typedef int TaskSequence;
typedef int ColourIndex;
typedef int Sphere;
typedef int ScrHandle;

#pragma pack(push, 1)
struct Vector3
{
	float x;
	DWORD _paddingx;
	float y;
	DWORD _paddingy;
	float z;
	DWORD _paddingz;

	static Vector3 Init(float x, float y, float z)
	{
		Vector3 result;
		result.x = x;
		result.y = y;
		result.z = z;
		return result;
	}

	Vector3 operator +(Vector3 val)
	{
		return Vector3::Init(
			this->x + val.x,
			this->y + val.y,
			this->z + val.z
		);
	}

	Vector3 operator -(Vector3 val)
	{
		return Vector3::Init(
			this->x - val.x,
			this->y - val.y,
			this->z - val.z);
	}

	Vector3 operator *(float val)
	{
		return Vector3::Init(
			this->x * val,
			this->y * val,
			this->z * val
		);
	}

	Vector3 operator /(float val)
	{
		return Vector3::Init(
			this->x / val,
			this->y / val,
			this->z / val
		);
	}

	static Vector3 Cross(Vector3 left, Vector3 right)
	{
		return Vector3::Init(
			left.y * right.z - left.z * right.y,
			left.z * right.x - left.x * right.z,
			left.x * right.y - left.y * right.x
		);
	}

	float Length()
	{
		return std::sqrt((this->x * this->x) + (this->y * this->y) + (this->z * this->z));
	}

	float distanceTo(Vector3 otherVector)
	{
		return (otherVector - *this).Length();
	}

	Vector3 GetDirectionForRotation()
	{
		double rotX = this->x / 57.295779513082320876798154814105;
		double rotZ = this->z / 57.295779513082320876798154814105;
		double multXY = abs(cos(rotX));
		return Vector3::Init(
			-sin(rotZ) * multXY,
			cos(rotZ) * multXY,
			sin(rotX)
		);
	}

};


#pragma pack(pop)