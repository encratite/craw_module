#include <Python.h>
#include <structmember.h>
#include "python.hpp"

namespace python
{
	PyMemberDef item_data_members[] =
	{
		{"id", T_UINT, offsetof(python_item_data, id), 0, "Item ID"},
		{"type", T_OBJECT, offsetof(python_item_data, type), 0, "Type name"},
		{"code", T_OBJECT, offsetof(python_item_data, code), 0, "Three letter item code"},
		{"quality", T_UBYTE, offsetof(python_item_data, quality), 0, "Quality"},
		{"level", T_UBYTE, offsetof(python_item_data, level), 0, "Item level"},
		{"sockets", T_UBYTE, offsetof(python_item_data, sockets), 0, "Number of sockets"},
		{"ethereal", T_OBJECT, offsetof(python_item_data, ethereal), 0, "Ethereal flag"},

		{0}
	};
}
