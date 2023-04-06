#pragma once

#include <string_view>

enum class OverrideShaderType
{
	LensDistortion,
	Snow
};

namespace Hooks
{
	void OverrideShader(OverrideShaderType shaderType, std::string_view shaderSrc);
	void ResetShader();
	void OnPresentCallback();
}