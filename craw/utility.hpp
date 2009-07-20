#pragma once

#include <string>
#include <ail/types.hpp>
#include <ail/string.hpp>
#include <windows.h>
#include <Tlhelp32.h>

extern unsigned page_size;
extern DWORD main_thread_id;

void error(std::string const & message);
void last_error(std::string const & message);
void initialise_console();
void write_text(std::string const & text);
void write_line(std::string const & text);
bool procedure_lookup(std::string const & module, std::string const & procedure, void * & output);
unsigned get_page(unsigned address);
void exit_process();
void attach_point();

typedef std::vector<DWORD> thread_id_vector;

class thread_controller
{
public:
	bool suspend();
	bool resume();

private:
	thread_id_vector thread_ids;
	DWORD current_thread_id;
	DWORD process_id;

	bool get_thread_ids();
	void process_thread_entry(THREADENTRY32 & thread_entry);
};

std::string dword_to_string(ulong input);
std::string wchar_to_string(wchar_t * input);

template <class type>
	void print_object(type & object)
{
	write_line("Printing object of size " + ail::number_to_string(sizeof(type)) + " at address " + ail::hex_string_32(reinterpret_cast<ulong>(&object)));
	std::string data(reinterpret_cast<char *>(&object), sizeof(type));
	write_line(ail::hex_string(data));
}
