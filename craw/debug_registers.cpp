#include <ail/string.hpp>
#include "debug_registers.hpp"
#include "utility.hpp"

namespace debug_register_data_type
{
	uchar const
		execute = 0,
		write = 1,
		read_write = 3;
}

namespace debug_register_data_size
{
	uchar const
		one = 0,
		two = 1,
		four = 3;
}

debug_register_data::debug_register_data()
{
}

debug_register_data::debug_register_data(unsigned address):
	address(address),
	type(debug_register_data_type::execute),
	size(debug_register_data_size::one)
{
}

debug_register_data::debug_register_data(unsigned address, uchar type, uchar size):
	address(address),
	type(type),
	size(size)
{
}

void set_debug_registers(CONTEXT & thread_context, unsigned address)
{
	debug_register_vector entries;
	entries.push_back(debug_register_data(address, debug_register_data_type::execute, debug_register_data_size::one));
	set_debug_registers(thread_context, entries);
}

void set_debug_registers(CONTEXT & thread_context, debug_register_vector & entries)
{
	DWORD * dr_addresses[] =
	{
		&thread_context.Dr0,
		&thread_context.Dr1,
		&thread_context.Dr2,
		&thread_context.Dr3
	};

	thread_context.Dr6 = 0;
	thread_context.Dr7 = 0;

	thread_context.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;

	for(std::size_t i = 0; i < 4 && i < entries.size(); i++)
	{
		debug_register_data & data = entries[i];
		*(dr_addresses[i]) = data.address;
		uchar high_group = (data.size << 2) | data.type;
		thread_context.Dr7 |= (high_group << (4 * i + 16)) | (1 << (2 * i));
	}

	for(std::size_t i = entries.size(); i < 4; i++)
		*(dr_addresses[i]) = 0;
}

void print_debug_registers(CONTEXT & thread_context)
{
	write_line("DR0: " + ail::hex_string_32(thread_context.Dr0));
	write_line("DR1: " + ail::hex_string_32(thread_context.Dr1));
	write_line("DR2: " + ail::hex_string_32(thread_context.Dr2));
	write_line("DR3: " + ail::hex_string_32(thread_context.Dr3));
	write_line("DR7: " + ail::hex_string_32(thread_context.Dr7));
}
