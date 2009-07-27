#include <boost/thread/mutex.hpp>
#include "python.hpp"
#include "d2_functions.hpp"
#include "utility.hpp"
#include "reveal_map.hpp"
#include "patch.hpp"

namespace python
{
	namespace
	{
		int const
			max_x = 800,
			max_y = 600;

		boost::mutex d2_function_mutex;
	}

	PyObject * set_handler(PyObject * self, PyObject * arguments, std::string const & name, PyObject * & output)
	{
		PyObject * new_handler;

		if(!PyArg_ParseTuple(arguments, ("O:" + name).c_str(), &new_handler))
			return 0;

		if(!PyCallable_Check(new_handler))
		{
			PyErr_SetString(PyExc_TypeError, "The handler must be a callable object.");
			return 0;
		}

		Py_XINCREF(new_handler);
		Py_XDECREF(output);
		output = output = new_handler;

		Py_RETURN_NONE;
	}
	
	PyObject * set_automap_handler(PyObject * self, PyObject * arguments)
	{
		return set_handler(self, arguments, "set_automap_handler", automap_handler);
	}

	PyObject * set_packet_handler(PyObject * self, PyObject * arguments)
	{
		return set_handler(self, arguments, "set_packet_handler", packet_handler);
	}

	PyObject * set_command_handler(PyObject * self, PyObject * arguments)
	{
		return set_handler(self, arguments, "set_command_handler", command_handler);
	}

	PyObject * set_keyboard_handler(PyObject * self, PyObject * arguments)
	{
		return set_handler(self, arguments, "set_keyboard_handler", keyboard_handler);
	}

	PyObject * set_bncs_packet_handler(PyObject * self, PyObject * arguments)
	{
		return set_handler(self, arguments, "set_bncs_packet_handler", bncs_packet_handler);
	}

	PyObject * set_item_handler(PyObject * self, PyObject * arguments)
	{
		return set_handler(self, arguments, "set_item_handler", item_handler);
	}

	void fix_coordinate(int & coordinate, int maximum)
	{
		if(coordinate < 0)
			coordinate = 0;
		if(coordinate >= maximum)
			coordinate = maximum - 1;
	}

	PyObject * draw_line(PyObject * self, PyObject * arguments)
	{
		int
			start_x,
			start_y,
			end_x,
			end_y,
			colour;

		if(!PyArg_ParseTuple(arguments, "iiiii", &start_x, &start_y, &end_x, &end_y, &colour))
			return 0;

		fix_coordinate(start_x, max_x);
		fix_coordinate(start_y, max_y);

		fix_coordinate(end_x, max_x);
		fix_coordinate(end_y, max_y);

		colour &= 0xff;

		boost::mutex::scoped_lock lock(d2_function_mutex);

		d2_draw_line(start_x, start_y, end_x, end_y, colour, 0xff);

		Py_RETURN_NONE;
	}

	PyObject * draw_text(PyObject * self, PyObject * arguments)
	{
		char * text;

		int
			x,
			y;

		PyObject * bool_object;

		if(!PyArg_ParseTuple(arguments, "sii|O", &text, &x, &y, &bool_object))
			return 0;

		fix_coordinate(x, max_x);
		fix_coordinate(y, max_y);

		bool centered = (bool_object == Py_True);

		boost::mutex::scoped_lock lock(d2_function_mutex);

		::draw_text(text, x, y, 0, centered);

		Py_RETURN_NONE;
	}

	PyObject * send_packet(PyObject * self, PyObject * arguments)
	{
		char const * data;
		Py_ssize_t size;

		if(!PyArg_ParseTuple(arguments, "s#", &data, &size))
			return 0;

		boost::mutex::scoped_lock lock(d2_function_mutex);

		d2_send_packet(static_cast<std::size_t>(size), 1, data);

		Py_RETURN_NONE;
	}

	PyObject * leave_game(PyObject * self, PyObject * arguments)
	{
		boost::mutex::scoped_lock lock(d2_function_mutex);

		d2_leave_game();
		Py_RETURN_NONE;
	}

	PyObject * get_life(PyObject * self, PyObject * arguments)
	{
		unsigned
			current_life,
			maximum_life;

		boost::mutex::scoped_lock lock(d2_function_mutex);

		if(!::get_life(current_life, maximum_life))
			Py_RETURN_NONE;

		PyObject * tuple = Py_BuildValue("(ii)", current_life, maximum_life);

		return tuple;
	}

	PyObject * get_mana(PyObject * self, PyObject * arguments)
	{
		unsigned
			current_mana,
			maximum_mana;

		boost::mutex::scoped_lock lock(d2_function_mutex);

		if(!::get_mana(current_mana, maximum_mana))
			Py_RETURN_NONE;

		PyObject * tuple = Py_BuildValue("(ii)", current_mana, maximum_mana);

		return tuple;
	}

	PyObject * get_player_level(PyObject * self, PyObject * arguments)
	{
		boost::mutex::scoped_lock lock(d2_function_mutex);

		unsigned level;
		if(!get_player_level_number(level))
			Py_RETURN_NONE;

		return PyLong_FromUnsignedLong(level);
	}

	PyObject * get_player_id(PyObject * self, PyObject * arguments)
	{
		boost::mutex::scoped_lock lock(d2_function_mutex);

		unsigned id;
		if(!::get_player_id(id))
			Py_RETURN_NONE;

		return PyLong_FromUnsignedLong(id);
	}

	PyObject * get_name_by_id(PyObject * self, PyObject * arguments)
	{
		unsigned id;
		if(!PyArg_ParseTuple(arguments, "i", &id))
			return 0;

		boost::mutex::scoped_lock lock(d2_function_mutex);

		std::string name;
		if(!::get_name_by_id(id, name))
			Py_RETURN_NONE;

		PyObject * output = create_string(name);

		return output;
	}

	PyObject * get_players(PyObject * self, PyObject * arguments)
	{
		boost::mutex::scoped_lock lock(d2_function_mutex);

		roster_vector roster_units = get_roster_units();
		std::size_t player_count = roster_units.size();
		PyObject * output = create_list(player_count);;

		for(std::size_t i = 0; i < player_count; i++)
		{
			roster_unit & current_unit = roster_units[i];

			path_data * path_data_pointer = 0;

			unit * unit_pointer;
			if(get_unit_by_id(current_unit.unit_id, unit_player, unit_pointer))
			{
				path_data_pointer = unit_pointer->path_data_pointer;
				if(path_data_pointer == 0)
				{
					error("Encountered a NULL path data pointer in a player's unit structure");
					exit_process();
					return 0;
				}
			}

			python_player_data * player_pointer = PyObject_New(python_player_data, &player_data_type);

			python_player_data & player_object = *player_pointer;
			player_object.id = current_unit.unit_id;
			player_object.character_class = current_unit.class_id;
			player_object.level = current_unit.level;

			player_object.life = current_unit.life;
			player_object.level_id = current_unit.level_id;

			if(path_data_pointer)
			{
				player_object.x = path_data_pointer->position_x;
				player_object.y = path_data_pointer->position_y;
			}
			else
			{
				player_object.x = current_unit.position_x;
				player_object.y = current_unit.position_y;
			}

			std::string name = current_unit.get_name();
			PyObject * string = create_string(name);

			player_object.name = string;

			set_list_item(output, i, reinterpret_cast<PyObject *>(player_pointer));
		}

		return output;
	}

	PyObject * get_player_location(PyObject * self, PyObject * arguments)
	{
		boost::mutex::scoped_lock lock(d2_function_mutex);

		unit * unit_pointer = d2_get_player_unit();
		if(unit_pointer == 0)
			Py_RETURN_NONE;

		return Py_BuildValue("(ii)", unit_pointer->x, unit_pointer->y);
	}

	PyObject * get_tp_tome_id(PyObject * self, PyObject * arguments)
	{
		boost::mutex::scoped_lock lock(d2_function_mutex);

		unsigned id;
		if(!get_non_empty_tp_tome_id(id))
			Py_RETURN_NONE;

		return PyLong_FromUnsignedLong(id);
	}

	PyObject * reveal_act(PyObject * self, PyObject * arguments)
	{
		boost::mutex::scoped_lock lock(d2_function_mutex);

		if(::reveal_act())
			Py_RETURN_TRUE;
		else
			Py_RETURN_FALSE;
	}

	PyObject * receive_packet(PyObject * self, PyObject * arguments)
	{
		char const * data;
		Py_ssize_t size;

		if(!PyArg_ParseTuple(arguments, "s#", &data, &size))
			return 0;

		boost::mutex::scoped_lock lock(d2_function_mutex);

		d2_receive_packet(data, size);

		Py_RETURN_NONE;
	}

	PyObject * send_bncs_packet(PyObject * self, PyObject * arguments)
	{
		char const * data;
		Py_ssize_t size;

		if(!PyArg_ParseTuple(arguments, "s#", &data, &size))
			return 0;

		boost::mutex::scoped_lock lock(d2_function_mutex);

		std::string packet(data, size);
		send_bncs_data(packet);

		Py_RETURN_NONE;
	}

	PyObject * move_click(PyObject * self, PyObject * arguments)
	{
		int
			x,
			y;

		if(!PyArg_ParseTuple(arguments, "ii", &x, &y))
			return 0;

		boost::mutex::scoped_lock lock(d2_function_mutex);

		::move_click(x, y);

		Py_RETURN_NONE;
	}

	PyObject * get_skill_level(PyObject * self, PyObject * arguments)
	{
		unsigned skill;

		if(!PyArg_ParseTuple(arguments, "i", &skill))
			return 0;

		boost::mutex::scoped_lock lock(d2_function_mutex);

		unsigned level;
		if(::get_skill_level(skill, level))
			return PyLong_FromUnsignedLong(level);

		Py_RETURN_NONE;
	}

	PyObject * get_minions(PyObject * self, PyObject * arguments)
	{
		unsigned player_id;

		if(!PyArg_ParseTuple(arguments, "i", &player_id))
			return 0;

		boost::mutex::scoped_lock lock(d2_function_mutex);

		std::vector<unit> minions;

		if(!::get_minions(player_id, minions))
			Py_RETURN_NONE;

		std::size_t minion_count = minions.size();

		PyObject * output = create_list(minion_count);
		for(std::size_t i = 0; i < minion_count; i++)
		{
			unit & current_unit = minions[i];
			python_monster_data * monster_data_pointer = PyObject_New(python_monster_data, &monster_data_type);
			python_monster_data & current_monster_data = *monster_data_pointer;
			if(!current_monster_data.initialise(current_unit))
				exit_process();
			set_list_item(output, i, reinterpret_cast<PyObject *>(monster_data_pointer));
		}

		return output;
	}
}
