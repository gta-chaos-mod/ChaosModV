#pragma once

#include <WinUser.h>
#include <wtypes.h>

// Code generously donated by stackoverflow: https://stackoverflow.com/questions/1888863/how-to-get-main-window-handle-from-process-id

struct handle_data
{
	unsigned long process_id;
	HWND window_handle;
};

BOOL is_main_window(HWND handle)
{
	return GetWindow(handle, GW_OWNER) == (HWND)0 && IsWindowVisible(handle);
}

BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
{
	handle_data& data = *(handle_data*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(handle, &process_id);

	if (data.process_id != process_id || !is_main_window(handle))
		return TRUE;

	data.window_handle = handle;
	return FALSE;
}

HWND find_main_window(unsigned long process_id)
{
	handle_data data{};
	data.process_id = process_id;
	data.window_handle = 0;
	EnumWindows(enum_windows_callback, (LPARAM)&data);
	return data.window_handle;
}

namespace Util
{
	HWND GetGameWindowHandle()
	{
		static HWND handle = find_main_window(GetCurrentProcessId());
		return handle;
	}
}