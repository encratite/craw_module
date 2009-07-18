//http://docs.python.org/extending/extending.html
//http://starship.python.net/crew/mwh/toext/node20.html

#include <ail/string.hpp>
#include <ail/array.hpp>
#include <ail/file.hpp>
#include "python.hpp"
#include "d2_functions.hpp"
#include "arguments.hpp"
#include "utility.hpp"

namespace python
{
	PyObject
		* automap_handler = 0,
		* packet_handler = 0,
		* command_handler = 0,
		* keyboard_handler = 0;

	namespace
	{
		std::string const
			module_name = "craw",
			error_name = "error",
			monster_data_class = "monster_data";

		PyObject * module_error;

		PyMethodDef module_methods[] =
		{
			{"set_automap_handler", &set_automap_handler, METH_VARARGS, "This function allows you to specify an automap unit handler which is called whenever a unit on the automap is being processed."},
			{"set_packet_handler", &set_packet_handler, METH_VARARGS, "This function allows you to specify a packet handler which can react to incoming packets from the game server."},
			{"set_command_handler", &set_command_handler, METH_VARARGS, "This function allows you to specify a console command handler which is called when the user has entered a command in the module's console."},
			{"set_keyboard_handler", &set_keyboard_handler, METH_VARARGS, "This function allows you to specify a keyboard handler which is called whenever a key is pressed in game when the player is not chatting."},

			{"draw_line", &draw_line, METH_VARARGS, "Draws a single line."},
			{"draw_text", &draw_text, METH_VARARGS, "Draws text on the screen."},

			{"send_packet", &send_packet, METH_VARARGS, "Sends a packet to the game server."},
			{"leave_game", &leave_game, METH_VARARGS, "Leaves the current game."},
			{"get_life", &get_life, METH_VARARGS, "Returns a (current life, maximum life) tuple or None if the character is not in game yet."},
			{"get_player_level", &get_player_level, METH_VARARGS, "Returns the numeric identifier of the area the player currently resides in or None if it failed to retrieve the identifier."},
			{"get_player_id", &get_player_id, METH_VARARGS, "Returns the numeric identifier of the player unit or None if it fails to retrieve it."},
			{"get_name_by_id", &get_name_by_id, METH_VARARGS, "Returns the name of a player given their numeric identifier or None if it fails to do so."},
			{"get_player_ids", &get_player_ids, METH_VARARGS, "Returns a list containing the IDs of all players that are currently in the game."},
			{"get_player_location", &get_player_location, METH_VARARGS, "Returns the player's location as a tuple (x, y) or None if the player is currently not in a game."},
			{"get_tp_tome_id", &get_tp_tome_id, METH_VARARGS, "Returns the ID of the first non-empty Tome of Town Portal in the player's inventory or None if it failed to find one."},

			{0, 0, 0, 0}
		};
	}
	
	PyMODINIT_FUNC initialise_module()
	{
		monster_data_type.tp_new = PyType_GenericNew;
		monster_data_type.tp_name = (module_name + "." + monster_data_class).c_str();
		if(PyType_Ready(&monster_data_type) < 0)
		{
			error("Failed to initialise the Python monster data type");
			return;
		}

		PyObject * module = Py_InitModule(module_name.c_str(), module_methods);
		if(module == 0)
			return;

		Py_INCREF(&monster_data_type);
		PyModule_AddObject(module, "monster_data", reinterpret_cast<PyObject *>(&monster_data_type));

		module_error = PyErr_NewException(const_cast<char *>((module_name + "." + error_name).c_str()), 0, 0);
		Py_INCREF(module_error);
		PyModule_AddObject(module, error_name.c_str(), module_error);
	}

	bool get_base_name(std::string const & input, std::string & output)
	{
		std::size_t offset = input.rfind('\\');
		if(offset == std::string::npos)
			return false;
		output = input.substr(0, offset);
		return true;
	}

	bool initialise_python()
	{
		if(python_script.empty())
			return true;

		std::string content;
		if(!ail::read_file(python_script, content))
		{
			error("Failed to load Python script \"" + python_script + "\"");
			return false;
		}

		if(prompt_mode)
			initialise_console();

		content = ail::replace_string(content, "\r", "");

		Py_Initialize();
		initialise_module();
		
		std::string script_directory;
		if(get_base_name(python_script, script_directory))
			PyRun_SimpleString(("import sys\nsys.path.append('" + script_directory + "')\n").c_str());

		if(PyRun_SimpleString(content.c_str()) != 0)
			return false;

		//PyRun_SimpleString(("execfile('" + ail::replace_string(python_script, "\\", "\\\\") + "')").c_str());

		return true;
	}
}
