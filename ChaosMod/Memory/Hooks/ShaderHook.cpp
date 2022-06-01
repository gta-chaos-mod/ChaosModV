#include <stdafx.h>

#include "ShaderHook.h"

#include "Memory/Hooks/Hook.h"
#include "Memory/Shader.h"

#define SHADER_CACHE_MAX_ENTRIES 20

using DWORD64 = unsigned long long;

static std::vector<BYTE> ms_rgShaderBytecode;
static EOverrideShaderType ms_eShaderType;

static bool ms_bRefreshShaders = false;

void *(*OG_rage__CreateShader)(const char *, BYTE *, DWORD, DWORD, DWORD *);
void *HK_rage__CreateShader(const char *name, BYTE *data, DWORD size, DWORD type, DWORD *out)
{
	if (!ms_rgShaderBytecode.empty())
	{
		const char *nameFilter = nullptr;
		switch (ms_eShaderType)
		{
		case EOverrideShaderType::LensDistortion:
			nameFilter = "PS_LensDistortion";
			break;
		case EOverrideShaderType::Snow:
			nameFilter = "PS_snow";
			break;
		}

		if (nameFilter && std::string_view(name).find(nameFilter) != std::string_view::npos)
		{
			return OG_rage__CreateShader(name, ms_rgShaderBytecode.data(), ms_rgShaderBytecode.size(), type, out);
		}
	}

	return OG_rage__CreateShader(name, data, size, type, out);
}

static bool OnHook()
{
	Handle handle;

	//

	handle = Memory::FindPattern("E8 ? ? ? ? 48 89 83 28 02 00 00 8B 44 24 30 89 83 30 02 00 00 EB 0F");
	if (!handle.IsValid())
	{
		return false;
	}

	Memory::AddHook(handle.Into().Get<void>(), HK_rage__CreateShader, &OG_rage__CreateShader);

	return true;
}

static RegisterHook registerHook(OnHook, nullptr, "ShaderHook", true);

namespace Hooks
{
	void OverrideShader(EOverrideShaderType shaderType, std::string_view szShaderSrc)
	{
		static std::unordered_map<size_t, std::vector<BYTE>> dictShaderCache;

		auto hash   = std::hash<std::string_view>()(szShaderSrc);
		auto result = dictShaderCache.find(hash);
		if (result == dictShaderCache.end())
		{
			auto future = std::async(
			    std::launch::async,
			    [&]
			    {
				    ID3DBlob *pShader;
				    ID3DBlob *pErrorMessages;
				    HRESULT compileResult;
				    if ((compileResult = D3DCompile(szShaderSrc.data(), szShaderSrc.size(), NULL, NULL, NULL, "main",
				                                    "ps_4_0", 0, 0, &pShader, &pErrorMessages))
				        == S_OK)
				    {
					    auto ptr = reinterpret_cast<BYTE *>(pShader->GetBufferPointer());
					    std::vector<BYTE> rgShaderBytecode;
					    rgShaderBytecode.reserve(pShader->GetBufferSize());
					    std::copy(ptr, ptr + pShader->GetBufferSize(), std::back_inserter(rgShaderBytecode));

					    if (dictShaderCache.size() > SHADER_CACHE_MAX_ENTRIES)
					    {
						    dictShaderCache.erase(dictShaderCache.begin());
					    }

					    dictShaderCache[hash] = rgShaderBytecode;
					    result                = dictShaderCache.find(hash);
				    }
				    else
				    {
					    LOG("Error compiling shader: Error code 0x" << std::hex << std::uppercase << compileResult);
					    if (pErrorMessages)
					    {
						    auto ptr = reinterpret_cast<BYTE *>(pErrorMessages->GetBufferPointer());
						    std::string buffer;
						    buffer.reserve(pErrorMessages->GetBufferSize());
						    std::copy(ptr, ptr + pErrorMessages->GetBufferSize(), std::back_inserter(buffer));

						    LOG(buffer);
						    return;
					    }
				    }
			    });

			using namespace std::chrono_literals;
			while (future.wait_for(0ms) != std::future_status::ready)
			{
				WAIT(0);
			}
		}

		if (result != dictShaderCache.end())
		{
			ms_rgShaderBytecode = result->second;
			ms_eShaderType      = shaderType;
			ms_bRefreshShaders  = true;
		}
	}

	void ResetShader()
	{
		ms_rgShaderBytecode.clear();
		ms_bRefreshShaders = true;
	}

	void OnPresentCallback()
	{
		if (ms_bRefreshShaders)
		{
			ms_bRefreshShaders = false;
			Memory::InvalidateShaderCache();
		}
	}
}