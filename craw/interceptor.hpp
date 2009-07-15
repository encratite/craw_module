#pragma once

#include <windows.h>

void initialise_dll_vector();
void perform_dll_check();
bool perform_debug_register_check(CONTEXT & thread_context);
bool perform_debug_register_trigger_check(HANDLE thread_handle, CONTEXT & thread_context, unsigned thread_target);
bool process_main_thread();
void hook_main_thread();
