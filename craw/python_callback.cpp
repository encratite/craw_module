#include <boost/thread/mutex.hpp>
#include "d2_functions.hpp"
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

		PyObject * packet_string = create_string(packet);

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

		PyObject * argument = create_string(line);

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

	void perform_item_callback(unit & current_item)
	{
		if(!item_handler)
			return;

		boost::mutex::scoped_lock lock(python_mutex);

		item_text * item_text_pointer = d2_get_item_text(current_item.table_index);
		if(item_text_pointer == 0)
		{
			error("Item text pointer is 0 for item " + ail::hex_string_32(reinterpret_cast<unsigned>(&current_item)));
			exit_process();
			return;
		}

		python_item_data * item_data_pointer = PyObject_New(python_item_data, &item_data_type);
		python_item_data & current_python_item_data = *item_data_pointer;

		current_python_item_data.id = current_item.id;

		wchar_t * unicode_name = get_unit_name(&current_item);
		std::string name = wchar_to_string(unicode_name);
		current_python_item_data.type = create_string(name);

		std::string code = item_text_pointer->get_code();
		current_python_item_data.code = create_string(code);

		item_data & current_item_data = *current_item.item_data_pointer;

		current_python_item_data.level = current_item_data.item_level;
		current_python_item_data.quality = current_item_data.quality;

		current_python_item_data.sockets = d2_get_unit_stat(&current_item, 0xc2, 0);

		current_python_item_data.ethereal = (current_item_data.flags & 0x00400000 ? Py_True : Py_False);

		Py_INCREF(current_python_item_data.ethereal);

		PyObject * return_value = PyObject_CallFunction(item_handler, "O", item_data_pointer);
		if(!return_value)
		{
			PyErr_Print();
			return;
		}

		Py_DECREF(current_python_item_data.ethereal);
		Py_XDECREF(current_python_item_data.type);
		Py_XDECREF(current_python_item_data.code);
		Py_XDECREF(item_data_pointer);
		Py_DECREF(return_value);
	}
}
