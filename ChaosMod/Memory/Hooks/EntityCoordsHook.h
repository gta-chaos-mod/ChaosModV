#pragma once

struct Vector3;

namespace Hooks
{
	void EnableFakeTpHook(const Vector3& prevPos);

	void DisableFakeTpHook();
}