#include "python.hpp"
#include "utility.hpp"

namespace python
{
	void perform_automap_callback(unit & current_unit, int x, int y, uchar colour)
	{
		if(!automap_handler)
			return;

		python_monster_data * monster_data_pointer = PyObject_New(python_monster_data, &monster_data_type);
		python_monster_data & current_monster_data = *monster_data_pointer;
		if(!current_monster_data.initialise(current_unit))
			exit_process();

		current_monster_data.x = x;
		current_monster_data.y = y;
		current_monster_data.colour = colour;

		PyObject * return_value = PyObject_CallFunction(automap_handler, "O", monster_data_pointer);

		Py_XDECREF(current_monster_data.treasure_class);
		Py_XDECREF(current_monster_data.special_abilities);
		Py_DECREF(monster_data_pointer);

		if(!return_value)
		{
			PyErr_Print();
			return;
		}

		Py_DECREF(return_value);
	}

	bool perform_packet_callback(std::string const & packet)
	{
		if(!packet_handler)
			return true;

		PyObject * return_value = PyObject_CallFunction(packet_handler, "s", packet.c_str());
		if(!return_value)
		{
			PyErr_Print();
			return true;
		}

		bool output = (return_value == Py_True);

		Py_DECREF(return_value);
		return output;
	}
}
