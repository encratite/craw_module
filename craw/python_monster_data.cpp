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
	PyMemberDef monster_data_members[] =
	{
		{"type", T_UINT, offsetof(python_monster_data, type), 0, "Unit type"},
		{"table_index", T_UINT, offsetof(python_monster_data, table_index), 0, "Offset into the monstats.bin table"},
		{"mode", T_UINT, offsetof(python_monster_data, mode), 0, "Mode of the monster"},

		{"monster_flags", T_UINT, offsetof(python_monster_data, monster_flags), 0, "Monster flags"},

		{"flags", T_UINT, offsetof(python_monster_data, flags), 0, "Monster statistics flags"},
		
		{"treasure_class", T_OBJECT, offsetof(python_monster_data, treasure_class), 0, "Treasure class list"},

		{"level", T_USHORT, offsetof(python_monster_data, level), 0, "Level"},
		{"min_hp", T_USHORT, offsetof(python_monster_data, min_hp), 0, "Minimum hit points"},
		{"max_hp", T_USHORT, offsetof(python_monster_data, max_hp), 0, "Maximum hit points"},
		{"experience", T_USHORT, offsetof(python_monster_data, experience), 0, "Experience"},

		{"damage_resistance", T_USHORT, offsetof(python_monster_data, damage_resistance), 0, "Damage resistance"},
		{"magic_resistance", T_USHORT, offsetof(python_monster_data, magic_resistance), 0, "Resistance to magical damage"},

		{"fire_resistance", T_USHORT, offsetof(python_monster_data, fire_resistance), 0, "Fire resistance"},
		{"lightning_resistance", T_USHORT, offsetof(python_monster_data, lightning_resistance), 0, "Lightning resistance"},
		{"cold_resistance", T_USHORT, offsetof(python_monster_data, cold_resistance), 0, "Cold resistance"},
		{"poison_resistance", T_USHORT, offsetof(python_monster_data, poison_resistance), 0, "Poison resistance"},

		{"special_abilities", T_OBJECT, offsetof(python_monster_data, special_abilities), 0, "Special abilities"},

		{"x", T_INT, offsetof(python_monster_data, x), 0, "Automap x coordinate"},
		{"y", T_INT, offsetof(python_monster_data, y), 0, "Automap y coordinate"},

		{"colour", T_UBYTE, offsetof(python_monster_data, colour), 0, "Colour"},

		{0}
	};

	PyTypeObject monster_data_type =
	{
		PyObject_HEAD_INIT(NULL)

		0,
		0,
		sizeof(python_monster_data),
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
		Py_TPFLAGS_DEFAULT,
		"Contains information about a Diablo II monster/NPC.",
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		monster_data_members,
		0,
		0,
		0,
		0,
		0,
		0,
		0
	};

	bool python_monster_data::initialise(unit & current_unit)
	{
		uchar difficulty = d2_get_difficulty();

		monster_statistics & statistics = get_monster_statistics(current_unit.table_index);
		monster_data & data = *reinterpret_cast<monster_data *>(current_unit.unit_data);

		type = current_unit.type;
		table_index = current_unit.table_index;
		mode = current_unit.mode;

		treasure_class = 0;
		special_abilities = 0;

		if(type == 1)
		{
			monster_flags = data.flags;

			flags = statistics.flags;

			treasure_class_entry current_treasure_class = statistics.treasure_classes[difficulty];

			std::size_t treasure_size = ail::countof(current_treasure_class.treasure_class);
			treasure_class = PyList_New(treasure_size);
			if(treasure_class == 0)
			{
				error("Failed to create the treasure class list");
				return false;
			}

			for(std::size_t i = 0; i < treasure_size; i++)
			{
				PyObject * integer = PyInt_FromLong(current_treasure_class.treasure_class[i]);
				if(PyList_SetItem(treasure_class, i, integer) < 0)
				{
					error("Failed to initialise a treasure class item");
					return false;
				}
			}

			std::vector<uchar> abilities_vector;

			BOOST_FOREACH(uchar ability, data.special_abilities)
			{
				if(ability != 0)
					abilities_vector.push_back(ability);
			}


			std::size_t vector_size = abilities_vector.size();

			special_abilities = PyList_New(vector_size);
			if(special_abilities == 0)
			{
				error("Failed to create the special abilities list");
				return false;
			}

			for(std::size_t i = 0; i < vector_size; i++)
			{
				PyObject * integer = PyInt_FromLong(abilities_vector[i]);
				if(PyList_SetItem(special_abilities, i, integer) < 0)
				{
					error("Failed to initialise a special abilities item");
					return false;
				}
			}

			level = statistics.level[difficulty];
			min_hp = statistics.min_hp[difficulty];
			max_hp = statistics.max_hp[difficulty];
			experience = statistics.experience[difficulty];

			ushort * resistance_pointers[] =
			{
				&damage_resistance,
				&magic_resistance,

				&fire_resistance,
				&lightning_resistance,
				&cold_resistance,
				&poison_resistance
			};

			unsigned resistance_stats[] =
			{
				36,
				37,

				39,
				41,
				43,
				45
			};

			for(std::size_t i = 0; i < ail::countof(resistance_pointers); i++)
				*resistance_pointers[i] = d2_get_unit_stat(&current_unit, resistance_stats[i], 0);

			/*
			damage_resistance = statistics.damage_resistance[difficulty];
			magic_resistance = statistics.magic_resistance[difficulty];

			fire_resistance = statistics.fire_resistance[difficulty];
			lightning_resistance = statistics.lightning_resistance[difficulty];
			cold_resistance = statistics.cold_resistance[difficulty];
			poison_resistance = statistics.poison_resistance[difficulty];
			*/
		}

		return true;
	}
}
