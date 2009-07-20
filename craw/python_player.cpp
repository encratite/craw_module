#include <Python.h>
#include <structmember.h>
#include <ail/array.hpp>
#include <ail/string.hpp>
#include <ail/types.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include "python.hpp"
#include "d2_functions.hpp"
#include "utility.hpp"

namespace python
{
	unsigned id;
		uchar character_class;
		uchar level; 
		PyObject * name;

	PyMemberDef player_data_members[] =
	{
		{"id", T_UINT, offsetof(python_player_data, id), 0, "Player ID"},
		{"character_class", T_UBYTE, offsetof(python_player_data, character_class), 0, "Character class"},
		{"level", T_UBYTE, offsetof(python_player_data, level), 0, "Character level"},
		{"name", T_OBJECT, offsetof(python_player_data, name), 0, "Name of the character"},

		{0}
	};
}
