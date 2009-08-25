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
#include "d2_functions.hpp"
#include "exception_handler.hpp"
#include "python.hpp"

namespace
{
	debug_register_vector main_debug_register_entries;

	unsigned
		packet_reception_interceptor_address,
		packet_reception_post_interceptor_address;

	std::size_t const
		packet_register_index = 2,
		add_unit_register_index = 3;
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
	main_debug_register_entries[packet_register_index] = debug_register_data(packet_reception_post_interceptor_address);
	set_debug_registers(thread_context, main_debug_register_entries);

	std::string packet(reinterpret_cast<char *>(thread_context.Ecx), static_cast<std::size_t>(thread_context.Edx));
	python::perform_packet_callback(packet);
}

void debug_register_post_receive_packet(CONTEXT & thread_context)
{
	main_debug_register_entries[packet_register_index] = debug_register_data(packet_reception_interceptor_address);
	set_debug_registers(thread_context, main_debug_register_entries);
}

void debug_register_add_unit1(CONTEXT & thread_context)
{
	main_debug_register_entries[add_unit_register_index] = debug_register_data(add_unit_address2);
	set_debug_registers(thread_context, main_debug_register_entries);

	unit & current_unit = *reinterpret_cast<unit *>(thread_context.Esi);
	if(current_unit.type != 4)
		return;

	python::perform_item_callback(current_unit);
}

void debug_register_add_unit2(CONTEXT & thread_context)
{
	main_debug_register_entries[add_unit_register_index] = debug_register_data(add_unit_address1);
	set_debug_registers(thread_context, main_debug_register_entries);
}

void debug_register_item_handler(CONTEXT & thread_context)
{
	unit & current_unit = *reinterpret_cast<unit *>(thread_context.Eax);
	python::perform_item_callback(current_unit);
	thread_context.Esi = thread_context.Eax;
	thread_context.Eip += 2;
}

int __fastcall unit_test(unit * unit_pointer)
{
	unit & current_unit = *unit_pointer;
	//if(current_unit.type == 0 && current_unit.mode != 0 && current_unit.mode != 0x11
	if(current_unit.type == 0)
		return 1;
	return -1;
}

void __declspec(naked) hackmap_code()
{
	__asm
	{
		/*
		shr eax, 015h
		and eax, 1
		add [esp], 3
		ret
		*/

		shr eax, 0x15
		and eax, 1
		jnz donothing
		mov ecx, ebp
		call unit_test
		cmp al, 0xFF
		jz quitcode
		jmp quitcode
		add dword ptr [esp], 0xA2 // adjust return address
		mov [esp + 4 + 0x34], eax // save return result
		pop eax // return address to eax
		push esi // push esi and edi because the return address skips these two instructions
		push edi
		jmp eax
donothing:
		ret
quitcode:
		xor eax, eax
		ret
	}
}

void debug_register_unit_selection(CONTEXT & thread_context)
{
	/*
	unit & current_unit = **reinterpret_cast<unit **>(thread_context.Esp + 4);
	if(current_unit.type == 0 && current_unit.mode != 0 && current_unit.mode != 0x11)
	{
		thread_context.Eax = 1;
		thread_context.Eip = *reinterpret_cast<unsigned *>(thread_context.Esp);
		thread_context.Esp += 5 * 4;
	}
	else
	{
		thread_context.Eax = *reinterpret_cast<unsigned *>(unit_selection_data_address);
		thread_context.Eip += 5;
	}*/

	thread_context.Esp -= 4;
	*reinterpret_cast<DWORD *>(thread_context.Esp) = thread_context.Eip + 5;
	thread_context.Eip = reinterpret_cast<DWORD>(&hackmap_code);
}

void d2net(unsigned base)
{
	initialise_d2net_addresses(base);
}

void d2client(unsigned base)
{
	initialise_d2client_addresses(base);

	packet_reception_interceptor_address = base + 0x65111;
	packet_reception_post_interceptor_address = packet_reception_interceptor_address + 5;

	//patch_address(automap_handler_address, &automap_blobs);

	debug_register_entries.push_back(debug_register_entry(light_handler_address, &debug_register_light));
	debug_register_entries.push_back(debug_register_entry(automap_loop_address, &debug_register_automap_loop));
	debug_register_entries.push_back(debug_register_entry(packet_reception_interceptor_address, &debug_register_receive_packet));
	debug_register_entries.push_back(debug_register_entry(packet_reception_post_interceptor_address, &debug_register_post_receive_packet));
	debug_register_entries.push_back(debug_register_entry(add_unit_address1, &debug_register_add_unit1));
	debug_register_entries.push_back(debug_register_entry(add_unit_address2, &debug_register_add_unit2));
	debug_register_entries.push_back(debug_register_entry(item_handler_call_address, &debug_register_item_handler));
	debug_register_entries.push_back(debug_register_entry(unit_is_selectable_address, &debug_register_unit_selection));

	main_debug_register_entries.push_back(debug_register_data(light_handler_address));
	if(!python_script.empty())
	{
		main_debug_register_entries.push_back(debug_register_data(automap_loop_address));
		main_debug_register_entries.push_back(debug_register_data(packet_reception_interceptor_address));
		//main_debug_register_entries.push_back(debug_register_data(add_unit_address1));
		main_debug_register_entries.push_back(debug_register_data(item_handler_call_address));
		//main_debug_register_entries.push_back(debug_register_data(unit_is_selectable_address));
	}

	set_own_context(main_debug_register_entries);
}

void bnclient(unsigned base)
{
	initialise_bnclient_addresses(base);
}

void initialise_dll_vector()
{
	dll_vector.push_back(dll_load_event("D2Win.dll", &initialise_d2win_addresses));
	dll_vector.push_back(dll_load_event("D2Gfx.dll", &initialise_d2gfx_addresses));
	dll_vector.push_back(dll_load_event("D2Common.dll", &initialise_d2common_addresses));
	dll_vector.push_back(dll_load_event("D2Client.dll", &d2client));

	dll_vector.push_back(dll_load_event("D2Net.dll", &d2net));
	dll_vector.push_back(dll_load_event("Bnclient.dll", &bnclient));
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
	HANDLE thread_handle = OpenThread(THREAD_SET_INFORMATION, FALSE, main_thread_id);
	if(thread_handle == 0)
	{
		last_error("Unable to open the main thread with the permissions required to change the thread priority");
		exit_process();
		return true;
	}

	if(SetThreadPriority(thread_handle, 0) == 0)
	{
		last_error("Unable to change thread priority");
		exit_process();
		return true;
	}
	CloseHandle(thread_handle);
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
