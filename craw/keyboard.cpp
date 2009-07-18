#include <windows.h>
#include <ail/string.hpp>
#include "keyboard.hpp"
#include "utility.hpp"
#include "d2_functions.hpp"
#include "patch.hpp"

namespace
{
	HHOOK keyboard_hook;
}

LRESULT CALLBACK keyboard_callback(int nCode, WPARAM wParam, LPARAM lParam)
{
	KBDLLHOOKSTRUCT & hook_struct = *reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);
	HWND current_window = GetForegroundWindow();

	write_line("Key activity: " + ail::bool_to_string(current_window == d2_window) + " " + ail::bool_to_string(d2_get_player_unit() != 0) + " " + ail::hex_string_32(hook_struct.vkCode));

	//check if this key activity was really for our Diablo II window and check if we are currently in a game
	if(current_window == d2_window && d2_get_player_unit())
	{
		write_line("Key activity: " + ail::hex_string_32(hook_struct.vkCode));
	}
	return CallNextHookEx(keyboard_hook, nCode, wParam, lParam);
}

bool install_keyboard_hook()
{
	/*
	HMODULE module_handle = GetModuleHandle(0);
	if(module_handle == 0)
	{
		last_error("Failed to retrieve module handle for keyboard hook");
		return false;
	}
	*/

	extern unsigned module_base;

	//keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, &keyboard_callback, reinterpret_cast<HINSTANCE>(module_handle), 0);
	keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, &keyboard_callback, reinterpret_cast<HINSTANCE>(module_base), 0);
	if(keyboard_hook == 0)
	{
		last_error("Failed to install the keyboard hook");
		return false;
	}

	/*
	MSG message;
	GetMessage(&message, 0, 0, 0);
	*/

	return true;
}
