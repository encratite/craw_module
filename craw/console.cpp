#include <iostream>
#include <vector>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <ail/string.hpp>
#include <ail/time.hpp>
#include <windows.h>
#include "console.hpp"
#include "d2_functions.hpp"
#include "reveal_map.hpp"
#include "utility.hpp"

//#define MAPHACK_TEST

#ifdef MAPHACK_TEST
//#include "gMap.h"
#include "mMap.h"
#endif

namespace
{
	console_command commands[] =
	{
		console_command("help", "", "Prints this help", 0, &print_help),
		console_command("?", "", "See 'help'", 0, &print_help),
		console_command("quit", "", "Terminates the program", 0, &quit_program),
		console_command("exit", "", "See 'quit'", 0, &quit_program),
		console_command("life", "", "Print your character's life", 0, &print_life),
		console_command("reveal", "", "Reveal the map of the current act your character is in", 0, &reveal_act_command),
		//console_command("test", "", "Perform maphack test", 0, &maphack_test),
		console_command("name", "", "Retrieve your character's name", 0, &get_character_name_command),
	};
}

console_command::console_command(std::string const & command, std::string const & argument_description, std::string const & description, std::size_t argument_count, command_handler handler):
	command(command),
	argument_description(argument_description),
	description(description),
	argument_count(argument_count),
	handler(handler)
{
}

bool console_command::match(std::string const & match_command, string_vector const & arguments) const
{
	return command == match_command && argument_count == arguments.size();
}

void print_life(string_vector const & arguments)
{
	unsigned
		current_life,
		maximum_life;

	/*
	__asm
	{
		int 3
	}
	*/

	if(get_life(current_life, maximum_life))
		std::cout << "Life: " << current_life << "/" << maximum_life << std::endl;
	else
		std::cout << "Your character is not in a game" << std::endl;
}

void maphack_test(string_vector const & arguments)
{
	#ifdef MAPHACK_TEST

	std::cout << "Running the maphack test" << std::endl;

	DefineOffsets();

	std::cout << "Defined the offsets" << std::endl;

	if(!GameReady())
	{
		std::cout << "The game is not ready" << std::endl;
		return;
	}

	thread_controller controller;
	if(!controller.suspend())
	{
		std::cout << "Failed to suspend all threads, resuming them" << std::endl;
		controller.resume();
		return;
	}

	std::cout << "Revealing the act" << std::endl;

	if(RevealAct())
		std::cout << "Done revealing the map" << std::endl;
	else
		std::cout << "Failed to reveal the act" << std::endl;

	#endif
}

void reveal_act_command(string_vector const & arguments)
{
	std::cout << "Revealing the map" << std::endl;

	bool const use_suspension = false;

	thread_controller controller;

	if(use_suspension && !controller.suspend())
	{
		std::cout << "Failed to suspend all threads, resuming them" << std::endl;
		controller.resume();
		return;
	}

	ullong start = ail::milliseconds();

	bool success = reveal_act();

	ullong duration = ail::milliseconds() - start;

	if(use_suspension && !controller.resume())
		return;

	if(success)
		std::cout << "Done revealing the map after " << duration << " ms" << std::endl;
	else
		std::cout << "Failed to reveal the act" << std::endl;
}

void get_character_name_command(string_vector const & arguments)
{
	std::cout << "Attempting to retrieve your character's name" << std::endl;
	unit * unit_pointer = d2_get_player_unit();
	if(unit_pointer == 0)
	{
		std::cout << "You do not appear to be in a game" << std::endl;
		return;
	}

	std::string name;
	if(get_name_by_id(unit_pointer->id, name))
		std::cout << "Your name is " << name << "." << std::endl;
	else
		std::cout << "Failed to retrieve your name." << std::endl;

	unsigned id;
	if(get_non_empty_tp_tome_id(id))
		std::cout << "Successfully retrieved the tome ID: " << ail::hex_string_32(id) << std::endl;
	else
		std::cout << "Failed to retrieve the tome ID" << std::endl;
}

void print_help(string_vector const & arguments)
{
	std::cout << "Available commands:" << std::endl;
	BOOST_FOREACH(console_command const & current_command, commands)
	{
		std::cout << std::endl << "Command: " << current_command.command << " " << current_command.argument_description << std::endl;
		std::cout << "Description: " << current_command.description << std::endl;
	}
}

void quit_program(string_vector const & arguments)
{
	std::cout << "Terminating..." << std::endl;
	ExitProcess(0);
}

void console_prompt()
{
	while(true)
	{
		std::cout << ">> ";
		std::string line;
		std::getline(std::cin, line);
		if(line.empty())
			continue;
		string_vector tokens = ail::tokenise(line, " ");
		if(tokens.empty())
			continue;

		std::string const & command = tokens[0];

		bool success = false;

		string_vector arguments = tokens;
		arguments.erase(arguments.begin());

		BOOST_FOREACH(console_command const & current_command, commands)
		{
			if(current_command.match(command, arguments))
			{
				current_command.handler(arguments);
				success = true;
				break;
			}
		}

		if(!success)
			std::cout << "Unknown command: \"" << command << "\"" << std::endl;
	}
}

void launch_prompt()
{
	//dirty!
	new boost::thread(&console_prompt);
}
