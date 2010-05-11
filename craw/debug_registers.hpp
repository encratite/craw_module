#pragma once

#include <vector>
#include <ail/types.hpp>
#include <windows.h>

namespace debug_register_data_type
{
	extern uchar const
		execute,
		write,
		read_write;
}

namespace debug_register_data_size
{
	extern uchar const
		one,
		two,
		four;
}

struct debug_register_data
{
	unsigned address;
	uchar type;
	uchar size;

	debug_register_data();
	debug_register_data(unsigned address);
	debug_register_data(unsigned address, uchar type, uchar size);
};

typedef std::vector<debug_register_data> debug_register_vector;

void set_debug_registers(CONTEXT & thread_context, unsigned address);
void set_debug_registers(CONTEXT & thread_context, debug_register_vector & addresses);
void print_debug_registers(CONTEXT & thread_context);
