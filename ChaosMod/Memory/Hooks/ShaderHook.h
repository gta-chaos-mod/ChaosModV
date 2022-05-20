#pragma once

#include <string_view>

enum class EOverrideShaderType
{
	LensDistortion,
	Snow
};

namespace Hooks
{
	void OverrideShader(EOverrideShaderType shaderType, std::string_view szShaderSrc);
	void ResetShader();
	void OnPresentCallback();
}