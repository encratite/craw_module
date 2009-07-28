#include <Python.h>
#include <structmember.h>
#include "python.hpp"

namespace python
{
	PyMemberDef player_data_members[] =
	{
		{"id", T_UINT, offsetof(python_player_data, id), 0, "Player ID"},
		{"character_class", T_UBYTE, offsetof(python_player_data, character_class), 0, "Character class"},
		{"level", T_USHORT, offsetof(python_player_data, level), 0, "Character level"},
		{"name", T_OBJECT, offsetof(python_player_data, name), 0, "Name of the character"},

		{"life", T_UINT, offsetof(python_player_data, life), 0, "Life"},
		{"level_id", T_UINT, offsetof(python_player_data, level_id), 0, "Level ID"},
		{"x", T_UINT, offsetof(python_player_data, x), 0, "x position"},
		{"y", T_UINT, offsetof(python_player_data, y), 0, "y position"},

		{"party", T_UINT, offsetof(python_player_data, party), 0, "Party ID"},

		{0}
	};
}
