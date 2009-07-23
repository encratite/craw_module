#include <boost/thread/mutex.hpp>
#include "python.hpp"
#include "utility.hpp"

namespace python
{
	namespace
	{
		boost::mutex python_mutex;
	}

	struct global_interpreter_lock
	{
		PyGILState_STATE gil_state;

		global_interpreter_lock()
		{
			gil_state = PyGILState_Ensure();
		}

		~global_interpreter_lock()
		{
			PyGILState_Release(gil_state);
		}
	};

	void perform_automap_callback(unit & current_unit, int x, int y, uchar colour)
	{
		if(!automap_handler)
			return;

		boost::mutex::scoped_lock lock(python_mutex);
		//global_interpreter_lock lock;

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

		boost::mutex::scoped_lock lock(python_mutex);
		//global_interpreter_lock lock;

		PyObject * packet_string = PyString_FromStringAndSize(packet.c_str(), packet.size());
		if(packet_string == 0)
		{
			error("Failed to create Python string object for the packet callback");
			exit_process();
			return true;
		}

		PyObject * return_value = PyObject_CallFunction(packet_handler, "O", packet_string);
		if(!return_value)
		{
			PyErr_Print();
			return true;
		}

		bool output = (return_value == Py_True);

		Py_DECREF(packet_string);
		Py_DECREF(return_value);

		return output;
	}

	bool perform_command_callback(std::string const & line)
	{
		if(!command_handler)
			return false;

		boost::mutex::scoped_lock lock(python_mutex);

		PyObject * argument = PyString_FromStringAndSize(line.c_str(), line.size());
		if(argument == 0)
		{
			error("Failed to create a Python string object for the console command");
			exit_process();
			return false;
		}

		PyObject * return_value = PyObject_CallFunction(command_handler, "O", argument);
		if(!return_value)
		{
			PyErr_Print();
			return false;
		}

		bool output = (return_value == Py_True);

		Py_DECREF(argument);
		Py_DECREF(return_value);

		return output;
	}

	bool perform_keyboard_callback(unsigned virtual_key)
	{
		if(!keyboard_handler)
			return false;

		boost::mutex::scoped_lock lock(python_mutex);

		PyObject * argument = PyLong_FromUnsignedLong(virtual_key);
		PyObject * return_value = PyObject_CallFunction(keyboard_handler, "O", argument);
		if(!return_value)
		{
			PyErr_Print();
			return false;
		}

		Py_XDECREF(argument);
		Py_DECREF(return_value);

		return true;
	}

	bool perform_bncs_callback(std::string const & packet)
	{
		if(!bncs_packet_handler)
			return false;

		boost::mutex::scoped_lock lock(python_mutex);

		PyObject * argument = PyString_FromStringAndSize(packet.c_str(), packet.size());
		PyObject * return_value = PyObject_CallFunction(bncs_packet_handler, "O", argument);
		if(!return_value)
		{
			PyErr_Print();
			return false;
		}

		Py_XDECREF(argument);
		Py_DECREF(return_value);

		return true;
	}
}
