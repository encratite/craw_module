#include <iostream>
#include <windows.h>
#include <ail/string.hpp>
#include <boost/foreach.hpp>
#include "utility.hpp"
#include "arguments.hpp"
#include "console.hpp"

unsigned page_size = 0x1000;
DWORD main_thread_id;

namespace
{
	HANDLE console_handle = 0;
}

void error(std::string const & message)
{
	MessageBox(0, message.c_str(), "Error", MB_OK);
}

void last_error(std::string const & message)
{
	error(message + ": " + ail::number_to_string(GetLastError()));
}

void initialise_console()
{
	if(console_handle == 0)
	{
		AllocConsole();
		console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		freopen("conin$", "r", stdin);
		freopen("conout$", "w", stdout);
		freopen("conout$", "w", stderr);
		std::cout.sync_with_stdio();
		std::cin.sync_with_stdio();

		if(prompt_mode)
			launch_prompt();
	}
}

void write_text(std::string const & text)
{
	if(console_output)
	{
		initialise_console();

		DWORD bytes_written;
		WriteConsole(console_handle, reinterpret_cast<void const *>(text.c_str()), static_cast<DWORD>(text.size()), &bytes_written, 0);
	}
}

void write_line(std::string const & text)
{
	write_text(text + "\n");
}

bool procedure_lookup(std::string const & module, std::string const & procedure, void * & output)
{
	HMODULE module_handle = GetModuleHandle(module.c_str());
	if(module_handle == 0)
	{
		write_line("Unable to get the module handle of " + module);
		return false;
	}

	output = reinterpret_cast<void *>(GetProcAddress(module_handle, procedure.c_str()));
	if(output == 0)
	{
		write_line("Unable to retrieve the address of " + procedure + " in " + module);
		return false;
	}

	return true;
}

unsigned get_page(unsigned address)
{
	return address & ~(page_size - 1);
}

std::string number_to_string(ulong input, ulong size)
{
	std::string output;
	for(ulong i = 0; i < size; i++)
		output.push_back(static_cast<char>((input >> (i * 8)) & 0xff));
	return output;
}

std::string dword_to_string(ulong input)
{
	return number_to_string(input, 4);
}

void exit_process()
{
	ExitProcess(1);
}

void attach_point()
{
	error("Attach");

	__asm
	{
		int 3
	}
}

bool thread_controller::suspend()
{
	current_thread_id = GetCurrentThreadId();

	if(!get_thread_ids())
		return false;

	BOOST_FOREACH(DWORD thread_id, thread_ids)
	{
		//write_line("Suspending thread with ID " + ail::hex_string_32(thread_id));

		HANDLE thread_handle = OpenThread(THREAD_SUSPEND_RESUME, FALSE, thread_id);
		if(thread_handle == 0)
		{
			last_error("Failed to open thread for suspension");
			return false;
		}

		if(SuspendThread(thread_handle) == -1)
		{
			last_error("Failed to suspend thread");
			CloseHandle(thread_handle);
			return false;
		}

		CloseHandle(thread_handle);
	}

	return true;
}

bool thread_controller::resume()
{
	BOOST_FOREACH(DWORD thread_id, thread_ids)
	{
		//write_line("Resuming thread with ID " + ail::hex_string_32(thread_id));

		HANDLE thread_handle = OpenThread(THREAD_SUSPEND_RESUME, FALSE, thread_id);
		if(thread_handle == 0)
		{
			last_error("Failed to open thread to resume its execution");
			return false;
		}

		if(ResumeThread(thread_handle) == -1)
		{
			last_error("Failed to resume thread");
			CloseHandle(thread_handle);
			return false;
		}

		CloseHandle(thread_handle);
	}

	return true;
}

bool thread_controller::get_thread_ids()
{
	THREADENTRY32 thread_entry;
	thread_entry.dwSize = static_cast<DWORD>(sizeof(THREADENTRY32));

	process_id = GetCurrentProcessId();

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if(snapshot == INVALID_HANDLE_VALUE)
	{
		last_error("Failed to create toolhelp snapshot");
		return false;
	}

	if(Thread32First(snapshot, &thread_entry) == FALSE)
	{
		last_error("Failed to retrieve the first snapshot");
		CloseHandle(snapshot);
		return false;
	}

	process_thread_entry(thread_entry);

	while(true)
	{
		if(Thread32Next(snapshot, &thread_entry) == FALSE)
			break;

		process_thread_entry(thread_entry);
	}

	CloseHandle(snapshot);
	return true;
}

void thread_controller::process_thread_entry(THREADENTRY32 & thread_entry)
{
	if(process_id != thread_entry.th32OwnerProcessID)
		return;

	DWORD thread_id = thread_entry.th32ThreadID;
	if(thread_id == current_thread_id || thread_id == main_thread_id)
		return;

	thread_ids.push_back(thread_id);
}

std::string wchar_to_string(wchar_t * input)
{
	char const * default_char = "_";
	int size = WideCharToMultiByte(CP_ACP, 0, input, -1, 0, 0, default_char, 0);
	char * buffer = new char[size];
	WideCharToMultiByte(CP_ACP, 0, input, -1, buffer, size, default_char, 0);
	std::string output(buffer, static_cast<std::size_t>(size - 1));
	delete buffer;
	return output;
}

wchar_t * string_to_wchar(std::string const & input)
{
	int length = MultiByteToWideChar(CP_ACP, 0, input.c_str(), -1, 0, 0);
	wchar_t * buffer = new wchar_t[length];
	MultiByteToWideChar(CP_ACP, 0, input.c_str(), -1, buffer, length);
	return buffer;
}
