#include "python.hpp"
#include "d2_functions.hpp"
#include "utility.hpp"

namespace python
{
	namespace
	{
		int const
			max_x = 800,
			max_y = 600;
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

		::draw_text(text, x, y, 0, centered);

		Py_RETURN_NONE;
	}

	PyObject * send_packet(PyObject * self, PyObject * arguments)
	{
		char const * data;
		Py_ssize_t size;

		if(!PyArg_ParseTuple(arguments, "s#", &data, &size))
			return 0;

		d2_send_packet(static_cast<std::size_t>(size), 1, data);

		Py_RETURN_NONE;
	}

	PyObject * leave_game(PyObject * self, PyObject * arguments)
	{
		d2_leave_game();
		Py_RETURN_NONE;
	}

	PyObject * get_life(PyObject * self, PyObject * arguments)
	{
		unsigned
			current_life,
			maximum_life;

		if(!::get_life(current_life, maximum_life))
			Py_RETURN_NONE;

		PyObject * tuple = Py_BuildValue("(ii)", current_life, maximum_life);

		return tuple;
	}

	PyObject * get_player_level(PyObject * self, PyObject * arguments)
	{
		unsigned level;
		if(!get_player_level_number(level))
			Py_RETURN_NONE;

		return PyLong_FromUnsignedLong(level);
	}

	PyObject * get_player_id(PyObject * self, PyObject * arguments)
	{
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

		std::string name;
		if(!::get_name_by_id(id, name))
			Py_RETURN_NONE;

		PyObject * output = PyString_FromStringAndSize(name.c_str(), name.size());
		if(output == 0)
		{
			error("Failed to create a Python string object to store the name of a player retrieved by their ID");
			exit_process();
			return 0;
		}

		return output;
	}

	PyObject * get_players(PyObject * self, PyObject * arguments)
	{
		roster_vector roster_units = get_roster_units();
		std::size_t player_count = roster_units.size();
		PyObject * output = PyList_New(player_count);
		if(output == 0)
		{
			error("Failed to create a Python list to store the player data");
			exit_process();
			return 0;
		}

		for(std::size_t i = 0; i < player_count; i++)
		{
			roster_unit & current_unit = roster_units[i];

			python_player_data * player_pointer = PyObject_New(python_player_data, &player_data_type);

			python_player_data & player_object = *player_pointer;
			player_object.id = current_unit.unit_id;
			player_object.character_class = current_unit.class_id;
			player_object.level = current_unit.level;

			player_object.life = current_unit.life;
			player_object.level_id = current_unit.level_id;
			player_object.x = current_unit.position_x;
			player_object.y = current_unit.position_y;

			std::string name = current_unit.get_name();
			PyObject * string = PyString_FromStringAndSize(name.c_str(), name.size());
			if(string == 0)
			{
				error("Failed to create a Python string object for a player's name");
				exit_process();
				return 0;
			}

			player_object.name = string;

			if(PyList_SetItem(output, i, reinterpret_cast<PyObject *>(player_pointer)) < 0)
			{
				error("Failed to set an element of the player data list");
				exit_process();
				return 0;
			}
		}

		return output;
	}

	PyObject * get_player_location(PyObject * self, PyObject * arguments)
	{
		unit * unit_pointer = d2_get_player_unit();
		if(unit_pointer == 0)
			Py_RETURN_NONE;

		return Py_BuildValue("(ii)", unit_pointer->x, unit_pointer->y);
	}

	PyObject * get_tp_tome_id(PyObject * self, PyObject * arguments)
	{
		unsigned id;
		if(!get_non_empty_tp_tome_id(id))
			Py_RETURN_NONE;

		return PyLong_FromUnsignedLong(id);
	}
}
