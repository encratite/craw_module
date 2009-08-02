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
		* keyboard_handler,
		* bncs_packet_handler,
		* item_handler;

	extern PyMemberDef
		monster_data_members[],
		player_data_members[],
		item_data_members[];

	extern PyTypeObject
		monster_data_type,
		player_data_type,
		item_data_type;

	struct python_monster_data
	{
		PyObject_HEAD

		unsigned id;

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

		PyObject * enchanted;

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

		unsigned party;
	};

	struct python_item_data
	{
		PyObject_HEAD

		unsigned id;
		PyObject * type;
		PyObject * code;
		uchar quality;
		uchar level;
		uchar sockets;
		PyObject * ethereal;
	};

	PyObject * create_list(std::size_t size);
	void set_list_item(PyObject * list, std::size_t index, PyObject * value);
	PyObject * create_string(std::string const & input);

	PyObject * set_automap_handler(PyObject * self, PyObject * arguments);
	PyObject * set_packet_handler(PyObject * self, PyObject * arguments);
	PyObject * set_command_handler(PyObject * self, PyObject * arguments);
	PyObject * set_keyboard_handler(PyObject * self, PyObject * arguments);
	PyObject * set_bncs_packet_handler(PyObject * self, PyObject * arguments);
	PyObject * set_item_handler(PyObject * self, PyObject * arguments);

	PyObject * draw_line(PyObject * self, PyObject * arguments);
	PyObject * draw_text(PyObject * self, PyObject * arguments);
	PyObject * send_packet(PyObject * self, PyObject * arguments);
	PyObject * leave_game(PyObject * self, PyObject * arguments);
	PyObject * get_life(PyObject * self, PyObject * arguments);
	PyObject * get_mana(PyObject * self, PyObject * arguments);
	PyObject * get_player_level(PyObject * self, PyObject * arguments);
	PyObject * get_player_id(PyObject * self, PyObject * arguments);
	PyObject * get_name_by_id(PyObject * self, PyObject * arguments);
	PyObject * get_player_location(PyObject * self, PyObject * arguments);
	PyObject * get_tp_tome_id(PyObject * self, PyObject * arguments);
	PyObject * get_players(PyObject * self, PyObject * arguments);
	PyObject * reveal_act(PyObject * self, PyObject * arguments);
	PyObject * receive_packet(PyObject * self, PyObject * arguments);
	PyObject * send_bncs_packet(PyObject * self, PyObject * arguments);
	PyObject * move_click(PyObject * self, PyObject * arguments);
	PyObject * get_skill_level(PyObject * self, PyObject * arguments);
	PyObject * get_minions(PyObject * self, PyObject * arguments);
	PyObject * print_text(PyObject * self, PyObject * arguments);
	PyObject * get_experience(PyObject * self, PyObject * arguments);

	void perform_automap_callback(unit & current_unit, int x, int y, uchar colour);
	bool perform_packet_callback(std::string const & packet);
	bool perform_command_callback(std::string const & line);
	bool perform_keyboard_callback(unsigned virtual_key);
	bool perform_bncs_callback(std::string const & packet);
	void perform_item_callback(unit & current_item);

	PyMODINIT_FUNC initialise_module();

	bool initialise_python();
}
