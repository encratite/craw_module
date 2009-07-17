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

		Py_INCREF(Py_None);
		return Py_None;
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

		return PyInt_FromLong(level);
	}
}
