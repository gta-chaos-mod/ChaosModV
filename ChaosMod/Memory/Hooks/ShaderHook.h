#pragma once

#include <string_view>

class IDXGISwapChain;

enum class EOverrideShaderType
{
    LensDistortion,
    Snow
};

namespace Hooks
{
    void OverrideShader(EOverrideShaderType shaderType, std::string_view szShaderSrc);
    void ResetShader();
    void OnPresentShaderHook(IDXGISwapChain* pSwapChain);
}