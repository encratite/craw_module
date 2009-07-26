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
		* keyboard_handler = 0,
		* bncs_packet_handler = 0,
		* item_handler = 0;

	PyTypeObject
		monster_data_type,
		player_data_type,
		item_data_type;

	namespace
	{
		std::string const
			module_name = "craw",
			error_name = "error";

		PyObject * module_error;

		PyMethodDef module_methods[] =
		{
			{"set_automap_handler", &set_automap_handler, METH_VARARGS, "This function allows you to specify an automap unit handler which is called whenever a unit on the automap is being processed."},
			{"set_packet_handler", &set_packet_handler, METH_VARARGS, "This function allows you to specify a packet handler which can react to incoming packets from the game server."},
			{"set_command_handler", &set_command_handler, METH_VARARGS, "This function allows you to specify a console command handler which is called when the user has entered a command in the module's console."},
			{"set_keyboard_handler", &set_keyboard_handler, METH_VARARGS, "This function allows you to specify a keyboard handler which is called whenever a key is pressed in game when the player is not chatting."},
			{"set_bncs_packet_handler", &set_bncs_packet_handler, METH_VARARGS, "This function allows you to specify a packet handler for incoming Battle.net Chat Server packets."},
			{"set_item_handler", &set_item_handler, METH_VARARGS, "This function allows you to specify an item handler which is called when new items are dropped."},

			{"draw_line", &draw_line, METH_VARARGS, "Draws a single line."},
			{"draw_text", &draw_text, METH_VARARGS, "Draws text on the screen."},

			{"send_packet", &send_packet, METH_VARARGS, "Sends a packet to the game server."},
			{"leave_game", &leave_game, METH_VARARGS, "Leaves the current game."},
			{"get_life", &get_life, METH_VARARGS, "Returns a (current life, maximum life) tuple or None if the character is not in game yet."},
			{"get_player_level", &get_player_level, METH_VARARGS, "Returns the numeric identifier of the area the player currently resides in or None if it failed to retrieve the identifier."},
			{"get_player_id", &get_player_id, METH_VARARGS, "Returns the numeric identifier of the player unit or None if it fails to retrieve it."},
			{"get_name_by_id", &get_name_by_id, METH_VARARGS, "Returns the name of a player given their numeric identifier or None if it fails to do so."},
			{"get_player_location", &get_player_location, METH_VARARGS, "Returns the player's location as a tuple (x, y) or None if the player is currently not in a game."},
			{"get_tp_tome_id", &get_tp_tome_id, METH_VARARGS, "Returns the ID of the first non-empty Tome of Town Portal in the player's inventory or None if it failed to find one."},
			{"get_players", &get_players, METH_VARARGS, "Returns a list of players currently in the game."},
			{"reveal_act", &reveal_act, METH_VARARGS, "Reveal the current act on the automap."},
			{"receive_packet", &receive_packet, METH_VARARGS, "Artificially receives a packet on the client side."},
			{"send_bncs_packet", &send_bncs_packet, METH_VARARGS, "Sends a packet to the Battle.net Chat Server."},
			{"move_click", &move_click, METH_VARARGS, "Move to the specified x, y coordinate."},

			{0, 0, 0, 0}
		};

		PyTypeObject default_type =
		{
			PyObject_HEAD_INIT(NULL)

			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0
		};
	}

	void print_python_type(PyTypeObject & input)
	{
		write_line(std::string("tp_name ") + (input.tp_name ? input.tp_name : "NULL"));
		write_line("tp_basicsize " + ail::number_to_string(input.tp_basicsize));
		write_line("tp_itemsize " + ail::number_to_string(input.tp_itemsize));

		write_line("tp_dealloc " + ail::hex_string_32((unsigned)input.tp_dealloc));
		write_line("tp_print " + ail::hex_string_32((unsigned)input.tp_print));
		write_line("tp_getattr " + ail::hex_string_32((unsigned)input.tp_getattr));
		write_line("tp_getattr " + ail::hex_string_32((unsigned)input.tp_getattr));
		write_line("tp_setattr " + ail::hex_string_32((unsigned)input.tp_setattr));
		write_line("tp_compare " + ail::hex_string_32((unsigned)input.tp_compare));
		write_line("tp_repr " + ail::hex_string_32((unsigned)input.tp_repr));

		write_line("tp_as_number " + ail::hex_string_32((unsigned)input.tp_as_number));
		write_line("tp_as_sequence " + ail::hex_string_32((unsigned)input.tp_as_sequence));
		write_line("tp_as_mapping " + ail::hex_string_32((unsigned)input.tp_as_mapping));

		write_line("tp_hash " + ail::hex_string_32((unsigned)input.tp_hash));
		write_line("tp_call " + ail::hex_string_32((unsigned)input.tp_call));
		write_line("tp_str " + ail::hex_string_32((unsigned)input.tp_str));
		write_line("tp_getattro " + ail::hex_string_32((unsigned)input.tp_getattro));
		write_line("tp_setattro " + ail::hex_string_32((unsigned)input.tp_setattro));

		write_line("tp_as_buffer " + ail::hex_string_32((unsigned)input.tp_as_buffer));

		write_line("tp_flags " + ail::number_to_string(input.tp_flags));

		write_line(std::string("tp_doc ") + (input.tp_doc ? input.tp_doc : "NULL"));

		write_line("tp_traverse " + ail::hex_string_32((unsigned)input.tp_traverse));

		write_line("tp_clear " + ail::hex_string_32((unsigned)input.tp_clear));

		write_line("tp_richcompare " + ail::hex_string_32((unsigned)input.tp_richcompare));

		write_line("tp_weaklistoffset " + ail::number_to_string(input.tp_weaklistoffset));

		write_line("tp_iter " + ail::hex_string_32((unsigned)input.tp_iter));
		write_line("tp_iternext " + ail::hex_string_32((unsigned)input.tp_iternext));

		write_line("tp_methods " + ail::hex_string_32((unsigned)input.tp_methods));
		write_line("tp_members " + ail::hex_string_32((unsigned)input.tp_members));
		write_line("tp_getset " + ail::hex_string_32((unsigned)input.tp_getset));
		write_line("tp_base " + ail::hex_string_32((unsigned)input.tp_base));

		write_line("tp_dict " + ail::hex_string_32((unsigned)input.tp_dict));
		write_line("tp_descr_get " + ail::hex_string_32((unsigned)input.tp_descr_get));
		write_line("tp_descr_set " + ail::hex_string_32((unsigned)input.tp_descr_set));

		write_line("tp_dictoffset " + ail::number_to_string(input.tp_dictoffset));

		write_line("tp_init " + ail::hex_string_32((unsigned)input.tp_init));
		write_line("tp_alloc " + ail::hex_string_32((unsigned)input.tp_alloc));
		write_line("tp_new " + ail::hex_string_32((unsigned)input.tp_new));
		write_line("tp_free " + ail::hex_string_32((unsigned)input.tp_free));
		write_line("tp_is_gc " + ail::hex_string_32((unsigned)input.tp_is_gc));
		write_line("tp_bases " + ail::hex_string_32((unsigned)input.tp_bases));
		write_line("tp_mro " + ail::hex_string_32((unsigned)input.tp_mro));
		write_line("tp_cache " + ail::hex_string_32((unsigned)input.tp_cache));
		write_line("tp_subclasses " + ail::hex_string_32((unsigned)input.tp_subclasses));
		write_line("tp_weaklist " + ail::hex_string_32((unsigned)input.tp_weaklist));
		write_line("tp_del " + ail::hex_string_32((unsigned)input.tp_del));

		write_line("tp_version_tag " + ail::number_to_string(input.tp_version_tag) + "\n\n");
	}

	void initialise_python_type(PyTypeObject & output, std::string const & class_name, std::size_t object_size, PyMemberDef * members, PyObject * module)
	{
		std::string full_name = module_name + "." + class_name;

		if(verbose)
			write_line("Adding type " + full_name + " to the Python module");

		char * name = new char[full_name.size() + 1];
		std::strcpy(name, full_name.c_str());

		output = default_type;
		output.tp_flags = Py_TPFLAGS_DEFAULT;
		output.tp_basicsize = object_size;
		output.tp_new = PyType_GenericNew;
		output.tp_name = name;
		output.tp_members = members;
		output.tp_doc = "No documentation available.";

		if(PyType_Ready(&output) < 0)
		{
			error("Failed to initialise Python data type \"" + class_name + "\"");
			exit_process();
		}

		Py_INCREF(&output);
		if(PyModule_AddObject(module, class_name.c_str(), reinterpret_cast<PyObject *>(&output)) != 0)
		{
			error("Failed to add type \"" + class_name + "\" to the Python module");
			exit_process();
		}
	}
	
	PyMODINIT_FUNC initialise_module()
	{
		PyObject * module = Py_InitModule(module_name.c_str(), module_methods);
		if(module == 0)
			return;

		initialise_python_type(monster_data_type, "monster_data", sizeof(python_monster_data), monster_data_members, module);
		initialise_python_type(player_data_type, "player_data", sizeof(python_player_data), player_data_members, module);
		initialise_python_type(item_data_type, "item_data", sizeof(python_item_data), item_data_members, module);

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
