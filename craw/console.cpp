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
#include "python.hpp"

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
		console_command("reveal", "", "Reveal the map of the act your character is currently in", 0, &reveal_act_command),
		//console_command("test", "", "Perform maphack test", 0, &maphack_test),
		console_command("name", "", "Retrieve your character's name", 0, &get_character_name_command),
		console_command("player", "", "Get player pointer", 0, &get_player_pointer),
		console_command("move", "<x> <y>", "Move to the specified coordinates.", 2, &move),
		console_command("pid", "", "Print the process ID", 0, &print_pid),
		console_command("test", "", "Run test function", 0, &run_test),
		console_command("print", " <text>", "Print chat text", -1, &print_text),
	};
}

console_command::console_command(std::string const & command, std::string const & argument_description, std::string const & description, long argument_count, command_handler handler):
	command(command),
	argument_description(argument_description),
	description(description),
	argument_count(argument_count),
	handler(handler)
{
}

bool console_command::match(std::string const & match_command, string_vector const & arguments) const
{
	return command == match_command && (argument_count == -1 || argument_count == arguments.size());
}

void print_life(string_vector const & arguments)
{
	unsigned
		current_life,
		maximum_life;

	if(get_life(current_life, maximum_life))
		std::cout << "Life: " << current_life << "/" << maximum_life << std::endl;
	else
		std::cout << "Your character is not in a game" << std::endl;
}

void get_player_pointer(string_vector const & arguments)
{
	unit * player_pointer = d2_get_player_unit();
	std::cout << ail::hex_string_32(reinterpret_cast<ulong>(player_pointer)) << std::endl;
	if(player_pointer && player_pointer->path_data_pointer)
	{
		path_data & data = *(player_pointer->path_data_pointer);
		std::cout << "Location: " << data.position_x << ", " << data.position_y << std::endl;
	}
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

void move(string_vector const & arguments)
{
	int
		x,
		y;

	if(!ail::string_to_number(arguments[0], x) || !ail::string_to_number(arguments[1], y))
	{
		std::cout << "Invalid coordinates specified" << std::endl;
		return;
	}

	//Sleep(2000);

	std::cout << "Moving to (" << x << ", " << y << ")" << std::endl;

	move_click(x, y);
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

void print_pid(string_vector const & arguments)
{
	std::cout << ail::hex_string_32(GetCurrentProcessId()) << std::endl;
}

void run_test(string_vector const & arguments)
{
	unit * unit_pointer = d2_get_player_unit();
	if(unit_pointer == 0)
	{
		std::cout << "Not in a game" << std::endl;
		return;
	}

	std::vector<unit> minions;
	if(!get_minions(unit_pointer->id, minions))
	{
		std::cout << "Failed to retrieve minions" << std::endl;
	}
	std::cout << "Counted " << minions.size() << " minion(s)" << std::endl;
}

void print_text(string_vector const & arguments)
{
	std::string text;
	bool first = true;
	BOOST_FOREACH(std::string const & argument, arguments)
	{
		if(first)
			first = false;
		else
			text += " ";
		text += argument;
	}
	std::cout << "Printing \"" << text << "\"" << std::endl;
	print_chat_text(text);
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
				if(command != "help")
					success = true;
				break;
			}
		}

		if(!success && !python::perform_command_callback(line))
			std::cout << "Unknown command: \"" << command << "\"" << std::endl;
	}
}

void launch_prompt()
{
	//dirty!
	new boost::thread(&console_prompt);
}
