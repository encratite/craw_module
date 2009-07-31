#include <windows.h>
#include <cstring>
#include <ail/arguments.hpp>
#include "arguments.hpp"
#include "utility.hpp"

bool
	console_output,
	incremental_window_title,
	verbose,
	use_custom_keys,
	prompt_mode,
	patch_system_modules;

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

namespace
{
	char * command_line_arguments;
}

std::string key_string(std::string const & key)
{
	if(key.empty())
		return "Key has not been specified";
	else
		return "A custom key has been specified";
}

bool install_command_line_patch(string_vector const & parsed_arguments)
{
	void * address;
	if(!procedure_lookup("kernel32.dll", "GetCommandLineA", address))
	{
		error("Unable to look up the address for the command line patch");
		return false;
	}

	char * byte_pointer = reinterpret_cast<char *>(address);
	uchar first_byte = *reinterpret_cast<uchar *>(byte_pointer);
	if(first_byte != 0xa1)
	{
		error("The format of the GetCommandlineA procedure in your kernel32.dll is unknown to the module's patcher and cannot be patched");
		return false;
	}

	char * & string_pointer = **reinterpret_cast<char ***>(byte_pointer + 1);
	std::string original_commandline(string_pointer);

	std::string new_arguments = "\"" + parsed_arguments[0] + "\" " + d2_arguments;
	char * cstring_arguments = new char[new_arguments.size() + 1];
	std::strcpy(cstring_arguments, new_arguments.c_str());

	string_pointer = cstring_arguments;

	if(verbose)
		write_line("Successfully installed the command line patch");
	
	return true;
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
