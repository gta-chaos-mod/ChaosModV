#include <stdafx.h>

#include "Memory/Hooks/Hook.h"
#include "Memory/Hooks/ShaderHook.h"

static void **ms_pPresentAddr = nullptr;

HRESULT (*OG_IDXGISwapChain_Present)(IDXGISwapChain *pSwapChain, UINT uiSyncInterval, UINT uiFlags);
HRESULT HK_IDXGISwapChain_Present(IDXGISwapChain *pSwapChain, UINT uiSyncInterval, UINT uiFlags)
{
	if (!(uiFlags & DXGI_PRESENT_TEST))
	{
		Hooks::OnPresentCallback();
	}

	return OG_IDXGISwapChain_Present(pSwapChain, uiSyncInterval, uiFlags);
}

static bool OnHook()
{
	Handle handle;

	handle = Memory::FindPattern("80 7E 10 00 48 8B");
	if (!handle.IsValid())
	{
		return false;
	}

	// IDXGISwapChain
	handle                    = *handle.At(6).Into().Value<DWORD64 *>();

	ms_pPresentAddr           = handle.At(64).Get<void *>();
	OG_IDXGISwapChain_Present = *(HRESULT(**)(IDXGISwapChain *, UINT, UINT))ms_pPresentAddr;
	Memory::Write<void *>(ms_pPresentAddr, HK_IDXGISwapChain_Present);

	return true;
}

static void OnCleanup()
{
	// Only reset vftable entries if address still points to our retour
	if (ms_pPresentAddr && *ms_pPresentAddr == HK_IDXGISwapChain_Present)
	{
		Memory::Write<void *>(ms_pPresentAddr, OG_IDXGISwapChain_Present);
	}
}

static RegisterHook registerHook(OnHook, OnCleanup, "IDXGISwapChain::Present", true);