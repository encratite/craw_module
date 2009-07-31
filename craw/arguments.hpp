#pragma once

#include <string>

extern bool
	console_output,
	incremental_window_title,
	verbose,
	use_custom_keys,
	prompt_mode,
	patch_system_modules;

extern std::string
	window_title,
	classic_key,
	expansion_key,
	d2_arguments,
	python_script,
	hide_modules_file,
	bncache_directory,
	socks;

extern bool use_socks;
extern std::string socks_server;
extern ushort socks_port;

void process_command_line();
