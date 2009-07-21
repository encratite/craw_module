#pragma once

#include <Python.h>
#include <string>
#include "d2_data.hpp"

namespace python
{
	extern PyObject
		* automap_handler,
		* packet_handler,
		* command_handler,
		* keyboard_handler;

	extern PyMemberDef
		monster_data_members[],
		player_data_members[];

	extern PyTypeObject
		monster_data_type,
		player_data_type;

	struct python_monster_data
	{
		PyObject_HEAD

		unsigned type;
		unsigned table_index;
		unsigned mode;

		unsigned monster_flags;

		unsigned flags;

		PyObject * treasure_class;

		ushort level;
		ushort min_hp;
		ushort max_hp;
		ushort experience;

		ushort damage_resistance;
		ushort magic_resistance;

		ushort fire_resistance;
		ushort lightning_resistance;
		ushort cold_resistance;
		ushort poison_resistance;

		PyObject * special_abilities;

		int x;
		int y;

		uchar colour;

		bool initialise(unit & current_unit);
	};

	struct python_player_data
	{
		PyObject_HEAD

		unsigned id;
		uchar character_class;
		ushort level; 
		PyObject * name;

		unsigned life;
		unsigned level_id;
		unsigned x;
		unsigned y;
	};

	PyObject * set_automap_handler(PyObject * self, PyObject * arguments);
	PyObject * set_packet_handler(PyObject * self, PyObject * arguments);
	PyObject * set_command_handler(PyObject * self, PyObject * arguments);
	PyObject * set_keyboard_handler(PyObject * self, PyObject * arguments);

	PyObject * draw_line(PyObject * self, PyObject * arguments);
	PyObject * draw_text(PyObject * self, PyObject * arguments);
	PyObject * send_packet(PyObject * self, PyObject * arguments);
	PyObject * leave_game(PyObject * self, PyObject * arguments);
	PyObject * get_life(PyObject * self, PyObject * arguments);
	PyObject * get_player_level(PyObject * self, PyObject * arguments);
	PyObject * get_player_id(PyObject * self, PyObject * arguments);
	PyObject * get_name_by_id(PyObject * self, PyObject * arguments);
	PyObject * get_player_location(PyObject * self, PyObject * arguments);
	PyObject * get_tp_tome_id(PyObject * self, PyObject * arguments);
	PyObject * get_players(PyObject * self, PyObject * arguments);
	PyObject * reveal_act(PyObject * self, PyObject * arguments);

	void perform_automap_callback(unit & current_unit, int x, int y, uchar colour);
	bool perform_packet_callback(std::string const & packet);
	bool perform_command_callback(std::string const & line);
	bool perform_keyboard_callback(unsigned virtual_key);

	PyMODINIT_FUNC initialise_module();

	bool initialise_python();
}
