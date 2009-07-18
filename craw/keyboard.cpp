#include <windows.h>
#include <ail/string.hpp>
#include "python.hpp"
#include "keyboard.hpp"
#include "d2_functions.hpp"
#include "utility.hpp"

namespace
{
	bool chat_box = false;
}

void process_key(unsigned virtual_key)
{
	if(!player_is_in_game())
		//only process key presses when the player is in game
		return;

	if(chat_box)
	{
		if(virtual_key == VK_RETURN || virtual_key == VK_ESCAPE)
			chat_box = false;
	}
	else
	{
		if(virtual_key == VK_RETURN)
			chat_box = true;
		else
		{
			//write_line("Key down: " + ail::hex_string_32(virtual_key));
			python::perform_keyboard_callback(virtual_key);
		}
	}
}
