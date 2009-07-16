#include <vector>
#include <iostream>
#include <ail/string.hpp>
#include <boost/foreach.hpp>
#include <windows.h>
#include <tlhelp32.h>
#include "patch.hpp"
#include "interceptor.hpp"
#include "utility.hpp"
#include "debug_registers.hpp"
#include "automap.hpp"
#include "arguments.hpp"
#include "packet_handler.hpp"
#include "d2_functions.hpp"
#include "exception_handler.hpp"

namespace
{
	unsigned original_game_packet_handler;
	unsigned original_receive_packet;

	unsigned
		packet_reception_interceptor_address,
		packet_reception_post_interceptor_address;

	debug_register_vector main_debug_register_entries;

	unsigned
		current_life,
		maximum_life;
}

typedef void (* initialisation_function_type)(unsigned base);
typedef void (* debug_register_event_handler_type)(CONTEXT & thread_context);

struct dll_load_event
{
	bool is_loaded;
	std::string dll_name;
	initialisation_function_type initialisation_function;

	dll_load_event()
	{
	}

	dll_load_event(std::string const & dll_name, initialisation_function_type initialisation_function):
		is_loaded(false),
		dll_name(dll_name),
		initialisation_function(initialisation_function)
	{
	}
};

struct debug_register_entry
{
	unsigned address;
	debug_register_event_handler_type handler;

	debug_register_entry()
	{
	}

	debug_register_entry(unsigned address, debug_register_event_handler_type handler):
		address(address),
		handler(handler)
	{
	}
};

struct debug_register_trigger
{
	std::string description;
	unsigned thread_target;
	debug_register_vector target_data;

	debug_register_trigger()
	{
	}

	debug_register_trigger(std::string const & description, unsigned thread_target, unsigned target_address):
		description(description),
		thread_target(thread_target)
	{
		target_data.push_back(debug_register_data(target_address));
	}
};

namespace
{
	std::vector<dll_load_event> dll_vector;
	std::vector<debug_register_entry> debug_register_entries;
	std::vector<debug_register_trigger> debug_register_triggers;
}

void actual_packet_handler(uchar * data, unsigned size)
{
	//write_line("Got a " + ail::hex_string_8(data[0]) + " packet (" + ail::number_to_string<unsigned>(size) + " byte(s))");
	//write_line(ail::hex_string(std::string(reinterpret_cast<char *>(data), static_cast<std::size_t>(size))));
}

void __declspec(naked) custom_packet_handler()
{
	__asm
	{
		pushad
		push ebx
		push ecx
		call actual_packet_handler
		add esp, 8
		popad
		sub esp, 0110h
		mov eax, original_game_packet_handler
		add eax, 6
		jmp eax
	}
}

void __declspec(naked) custom_light_handler()
{
	__asm
	{
		push ecx
		test eax, eax
		mov byte ptr [esp], 0
		mov byte ptr [esp + 1], 0
		mov byte ptr [esp + 2], 0
		jz other_branch
		mov eax, 0ffh
		mov byte ptr [esp], al
		mov byte ptr [esp + 1], al
		mov byte ptr [esp + 2], al
		mov ecx, light_handler_address
		add ecx, 08eh
		jmp ecx

	other_branch:
		mov eax, light_handler_address
		add eax, 0a5h
		jmp eax
	}
}

void debug_register_packet_handler(CONTEXT & thread_context)
{
	thread_context.Eip = reinterpret_cast<DWORD>(&main_packet_handler);
}

void debug_register_light(CONTEXT & thread_context)
{
	thread_context.Eip = reinterpret_cast<DWORD>(&custom_light_handler);
}

void debug_register_automap_loop(CONTEXT & thread_context)
{
	thread_context.Eip = reinterpret_cast<DWORD>(&automap_loop);
}

void debug_register_receive_packet(CONTEXT & thread_context)
{
	debug_register_vector debug_registers = main_debug_register_entries;
	debug_registers.push_back(debug_register_data(packet_reception_post_interceptor_address));
	set_debug_registers(thread_context, debug_registers);

	get_life(current_life, maximum_life);
}

void debug_register_post_receive_packet(CONTEXT & thread_context)
{
	debug_register_vector debug_registers = main_debug_register_entries;
	debug_registers.push_back(debug_register_data(packet_reception_interceptor_address));
	set_debug_registers(thread_context, debug_registers);

	unsigned
		new_current_life,
		new_maximum_life;

	get_life(new_current_life, new_maximum_life);

	if(current_life != new_current_life || maximum_life != new_maximum_life)
		write_line("\tCHANGE DETECTED: " + ail::number_to_string(current_life) + "/" + ail::number_to_string(new_maximum_life));
	else
		write_line("No change: " + ail::number_to_string(current_life) + "/" + ail::number_to_string(new_maximum_life));
}

void d2net(unsigned base)
{
	original_game_packet_handler = base + 0x6650;
	original_receive_packet = base + 0x6090;

	debug_register_entries.push_back(debug_register_entry(original_game_packet_handler, &debug_register_packet_handler));

	debug_register_triggers.push_back(debug_register_trigger("Game packet handler thread", base + 0x6bd0, original_game_packet_handler));

	//patch_address(original_game_packet_handler, &main_packet_handler);

	initialise_d2net_addresses(base);
}

void d2client(unsigned base)
{
	packet_reception_interceptor_address = base + 0x65111;
	packet_reception_post_interceptor_address = packet_reception_interceptor_address + 5;

	initialise_d2client_addresses(base);

	//patch_address(automap_handler_address, &automap_blobs);

	debug_register_entries.push_back(debug_register_entry(light_handler_address, &debug_register_light));
	debug_register_entries.push_back(debug_register_entry(automap_loop_address, &debug_register_automap_loop));
	debug_register_entries.push_back(debug_register_entry(packet_reception_interceptor_address, &debug_register_receive_packet));
	debug_register_entries.push_back(debug_register_entry(packet_reception_post_interceptor_address, &debug_register_post_receive_packet));

	main_debug_register_entries.push_back(debug_register_data(light_handler_address));
	main_debug_register_entries.push_back(debug_register_data(automap_loop_address));

	debug_register_vector debug_registers = main_debug_register_entries;
	//debug_registers.push_back(debug_register_data(packet_reception_interceptor_address));

	set_own_context(debug_registers);
}

void initialise_dll_vector()
{
	dll_vector.push_back(dll_load_event("D2Win.dll", &initialise_d2win_addresses));
	dll_vector.push_back(dll_load_event("D2Gfx.dll", &initialise_d2gfx_addresses));
	dll_vector.push_back(dll_load_event("D2Common.dll", &initialise_d2common_addresses));
	dll_vector.push_back(dll_load_event("D2Client.dll", &d2client));

	dll_vector.push_back(dll_load_event("D2Net.dll", &d2net));
}

void perform_dll_check()
{
	BOOST_FOREACH(dll_load_event & handler, dll_vector)
	{
		if(!handler.is_loaded)
		{
			unsigned base = reinterpret_cast<unsigned>(GetModuleHandle(handler.dll_name.c_str()));
			if(base != 0)
			{
				handler.is_loaded = true;
				if(verbose)
					write_line("Preparing the debug register interception for " + handler.dll_name + " (" + ail::hex_string_32(base) + ")");
				handler.initialisation_function(base);
			}
		}
	}
}

bool perform_debug_register_check(CONTEXT & thread_context)
{
	unsigned address = static_cast<unsigned>(thread_context.Eip);
	//write_line("Trying to match the address to a debug register handler: " + ail::hex_string_32(address));
	BOOST_FOREACH(debug_register_entry & entry, debug_register_entries)
	{
		if(entry.address != address)
			continue;

		//write_line("Discovered a debug register handler for address " + ail::hex_string_32(address));
		entry.handler(thread_context);
		return true;
	}

	error("Was unable to find a debug register handler for address " + ail::hex_string_32(address));
	return false;
}

bool perform_debug_register_trigger_check(HANDLE thread_handle, CONTEXT & thread_context, unsigned thread_target)
{
	BOOST_FOREACH(debug_register_trigger & trigger, debug_register_triggers)
	{
		if(trigger.thread_target != thread_target)
			continue;

		set_debug_registers(thread_context, trigger.target_data);
		if(!SetThreadContext(thread_handle, &thread_context))
		{
			error("Failed to set thread context for target " + ail::hex_string_32(thread_target) + "!");
			return false;
		}

		if(verbose)
			write_line("Important thread detected: " + trigger.description);
		return true;
	}
	//write_line("This is not a relevant thread.");
	return false;
}

bool process_thread_entry(DWORD process_id, DWORD current_thread_id, THREADENTRY32 & thread_entry)
{
	if(process_id != thread_entry.th32OwnerProcessID)
		return false;
	DWORD thread_id = thread_entry.th32ThreadID;
	if(thread_id == current_thread_id)
		return false;

	if(verbose)
		write_line("Main thread: " + ail::hex_string_32(thread_id));
	main_thread_id = thread_id;
	return true;
}

bool process_main_thread()
{
	DWORD process_id = GetCurrentProcessId();
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if(snapshot == INVALID_HANDLE_VALUE)
	{
		last_error("CreateToolhelp32Snapshot failed");
		CloseHandle(snapshot);
		return false;
	}
	DWORD current_thread_id = GetCurrentThreadId();
	THREADENTRY32 thread_entry;
	thread_entry.dwSize = sizeof(THREADENTRY32);
	if(Thread32First(snapshot, &thread_entry) == FALSE)
	{
		last_error("Thread32First failed");
		CloseHandle(snapshot);
		return false;
	}
	bool success = process_thread_entry(process_id, current_thread_id, thread_entry);
	while(Thread32Next(snapshot, &thread_entry) == TRUE)
		success = success || process_thread_entry(process_id, current_thread_id, thread_entry);
	if(!success)
	{
		error("Unable to detect main thread!");
		CloseHandle(snapshot);
		return false;
	}
	CloseHandle(snapshot);
	return true;
}
