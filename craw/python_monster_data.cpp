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
		{"id", T_UINT, offsetof(python_monster_data, id), 0, "Unit identifier"},

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

		{"damage_resistance", T_SHORT, offsetof(python_monster_data, damage_resistance), 0, "Damage resistance"},
		{"magic_resistance", T_SHORT, offsetof(python_monster_data, magic_resistance), 0, "Resistance to magical damage"},

		{"fire_resistance", T_SHORT, offsetof(python_monster_data, fire_resistance), 0, "Fire resistance"},
		{"lightning_resistance", T_SHORT, offsetof(python_monster_data, lightning_resistance), 0, "Lightning resistance"},
		{"cold_resistance", T_SHORT, offsetof(python_monster_data, cold_resistance), 0, "Cold resistance"},
		{"poison_resistance", T_SHORT, offsetof(python_monster_data, poison_resistance), 0, "Poison resistance"},

		{"special_abilities", T_OBJECT, offsetof(python_monster_data, special_abilities), 0, "Special abilities"},

		{"x", T_INT, offsetof(python_monster_data, x), 0, "Automap x coordinate"},
		{"y", T_INT, offsetof(python_monster_data, y), 0, "Automap y coordinate"},

		{"colour", T_UBYTE, offsetof(python_monster_data, colour), 0, "Colour"},

		{"enchanted", T_OBJECT, offsetof(python_monster_data, enchanted), 0, "True if the monster is enchanted"},

		{0}
	};

	bool python_monster_data::initialise(unit & current_unit)
	{
		uchar difficulty = d2_get_difficulty();

		monster_statistics & statistics = get_monster_statistics(current_unit.table_index);
		monster_data & data = *reinterpret_cast<monster_data *>(current_unit.unit_data);

		id = current_unit.id;
		type = current_unit.type;
		table_index = current_unit.table_index;
		mode = current_unit.mode;

		treasure_class = 0;
		special_abilities = 0;
		enchanted = 0;

		if(type == 1)
		{
			monster_flags = data.flags;

			flags = statistics.flags;

			treasure_class_entry current_treasure_class = statistics.treasure_classes[difficulty];

			std::size_t treasure_size = ail::countof(current_treasure_class.treasure_class);
			treasure_class = create_list(treasure_size);

			for(std::size_t i = 0; i < treasure_size; i++)
			{
				PyObject * integer = PyLong_FromUnsignedLong(current_treasure_class.treasure_class[i]);
				set_list_item(treasure_class, i, integer);
			}

			std::vector<uchar> abilities_vector;

			BOOST_FOREACH(uchar ability, data.special_abilities)
			{
				if(ability != 0)
					abilities_vector.push_back(ability);
			}

			std::size_t vector_size = abilities_vector.size();

			special_abilities = create_list(vector_size);

			for(std::size_t i = 0; i < vector_size; i++)
			{
				PyObject * integer = PyLong_FromUnsignedLong(abilities_vector[i]);
				set_list_item(special_abilities, i, integer);
			}

			level = statistics.level[difficulty];
			min_hp = statistics.min_hp[difficulty];
			max_hp = statistics.max_hp[difficulty];
			experience = statistics.experience[difficulty];

			short * resistance_pointers[] =
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

			bool is_enchanted = d2_get_unit_state(&current_unit, 16) != 0;
			
			enchanted = is_enchanted ? Py_True : Py_False;

			Py_INCREF(enchanted);
		}

		return true;
	}
}
