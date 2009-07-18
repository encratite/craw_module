#pragma once

#include <string>

#include <windows.h>

extern HWND d2_window;

struct hot_patch_entry
{
	std::string
		module,
		procedure;

	void * function;
	void * & real_function;

	bool enabled;

	hot_patch_entry(std::string const & module, std::string const & procedure, void * function, void * & real_function, bool enabled = true);
};

bool hot_patch_function(std::string const & module, std::string const & function, void * function_pointer, void * & old_address);
bool patch_function(std::string const & module, std::string const & procedure, void * & address, void * custom_function);
bool patch_address(unsigned address, void * target);

bool apply_hot_patches();
bool patch_exception_handler();
