#include <windows.h>
#include <cstring>
#include <ail/arguments.hpp>
#include "arguments.hpp"
#include "utility.hpp"
#include "patch.hpp"

bool
	console_output,
	incremental_window_title,
	verbose,
	use_custom_keys,
	prompt_mode,
	patch_system_modules,
	do_not_hide_window;

std::string
	window_title,
	classic_key,
	expansion_key,
	d2_arguments,
	python_script,
	hide_modules_file,
	bncache_directory,
	socks;

bool use_socks;
std::string socks_server;
ushort socks_port;

char * command_line_arguments;

std::string key_string(std::string const & key)
{
	if(key.empty())
		return "Key has not been specified";
	else
		return "A custom key has been specified";
}

char * patched_GetCommandLineA()
{
	return command_line_arguments;
}

bool install_command_line_patch(string_vector const & parsed_arguments)
{
	OSVERSIONINFOEX version;
	ZeroMemory(&version, sizeof(OSVERSIONINFOEX));
	version.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if(!GetVersionEx(reinterpret_cast<OSVERSIONINFO *>(&version)))
	{
		last_error("GetVersionEx failed");
		return false;
	}
	if(verbose)
	{
		write_line("GetVersionEx information for this operating system:");
		write_line("dwMajorVersion: " + ail::hex_string_32(version.dwMajorVersion));
		write_line("dwMinorVersion: " + ail::hex_string_32(version.dwMinorVersion));
		write_line("wProductType: " + ail::hex_string_16(version.wProductType));
	}
	std::string kernel;
	if(version.dwMajorVersion == 6 && version.dwMinorVersion == 1 && version.wProductType == VER_NT_WORKSTATION)
	{
		kernel = "kernelbase.dll";
		if(verbose)
			write_line("Windows 7 detected, using module " + kernel + " for the commandline patch");
	}
	else
	{
		kernel = "kernel32.dll";
		if(verbose)
			write_line("Pre Windows 7 OS detected, using module " + kernel + " for the commandline patch");
	}

	void * address;
	if(!procedure_lookup(kernel.c_str(), "GetCommandLineA", address))
	{
		error("Unable to look up the address for the command line patch");
		return false;
	}

	std::string new_arguments = "\"" + parsed_arguments[0] + "\" " + d2_arguments;
	command_line_arguments = new char[new_arguments.size() + 1];
	std::strcpy(command_line_arguments, new_arguments.c_str());

	char * byte_pointer = reinterpret_cast<char *>(address);
	uchar first_byte = *reinterpret_cast<uchar *>(byte_pointer);
	if(first_byte == 0xa1)
	{
		char * & string_pointer = **reinterpret_cast<char ***>(byte_pointer + 1);
		string_pointer = command_line_arguments;

		if(verbose)
			write_line("Successfully installed the command line patch");
		
		return true;
	}
	else
	{
		error("The format of your " + kernel + "!GetCommandlineA is unknown to the patcher: " + ail::hex_string_8(first_byte));
		return false;
	}
}

bool get_arguments(string_vector & output)
{
	std::string input = GetCommandLine();
	for(std::size_t i = 0; i < input.size(); i++)
	{
		std::size_t
			start,
			end;

		char current_char = input[i];
		if(current_char == '"')
		{
			start = i + 1;
			end = input.find('"', start);
			if(end == std::string::npos)
			{
				error("Invalid commandline detected (check your quotes)");
				return false;
			}
		}
		else
		{
			start = i;
			end = input.find(' ', start);
			if(end == std::string::npos)
				end = input.size();
		}

		std::string argument = input.substr(start, end - start);
		output.push_back(argument);
		i = end + 1;
	}

	return true;
}

void process_command_line()
{
	ail::argument_parser argument_parser;

	argument_parser.flag("console_output", console_output).default_flag(true);
	argument_parser.flag("incremental_window_title", incremental_window_title).default_flag(false);
	argument_parser.flag("verbose", verbose).default_flag(false);
	argument_parser.flag("prompt", prompt_mode).default_flag(false);
	argument_parser.flag("patch_system_modules", patch_system_modules).default_flag(true);
	argument_parser.flag("do_not_hide_window", do_not_hide_window).default_flag(false);

	argument_parser.string("window_title", window_title).default_string("Diablo II");
	argument_parser.string("classic_key", classic_key);
	argument_parser.string("expansion_key", expansion_key);
	argument_parser.string("d2_arguments", d2_arguments).default_string("-w");
	argument_parser.string("python_script", python_script);
	argument_parser.string("hide_modules_file", hide_modules_file);
	argument_parser.string("bncache_directory", bncache_directory);
	argument_parser.string("socks", socks);

	string_vector arguments;
	if(!get_arguments(arguments))
	{
		exit_process();
		return;
	}

	try
	{
		argument_parser.parse(arguments);

		if(verbose)
		{
			write_line("Window title: " + window_title);
			write_line("Classic key: " + key_string(classic_key));
			write_line("Expansion key: " + key_string(expansion_key));
		}

		use_custom_keys = !classic_key.empty() && !expansion_key.empty();

		if(verbose)
		{
			if(use_custom_keys)
				write_line("Using the specified custom keys");
			else
				write_line("Not using a custom pair of keys");
		}

		install_command_line_patch(arguments);
	}
	catch(ail::exception & exception)
	{
		error("Argument parser error: " + exception.get_message());
	}

	use_socks = false;
	if(!socks.empty())
	{
		string_vector tokens = ail::tokenise(socks, ":");
		if(tokens.size() != 2)
		{
			error("Invalid SOCKS5 server address specified, should be \"host:port\", encountered \"" + socks + "\"");
			exit_process();
			return;
		}

		socks_server = tokens[0];
		std::string const & socks_port_string = tokens[1];
		if(!ail::string_to_number(socks_port_string, socks_port))
		{
			error("Invalid SOCKS5 server port specified: " + socks_port_string);
			exit_process();
			return;
		}
		use_socks = true;
	}
}
