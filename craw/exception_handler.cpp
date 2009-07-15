#include <iostream>
#include <windows.h>
#include <boost/thread/mutex.hpp>
#include <ail/string.hpp>
#include <ail/types.hpp>
#include "exception_handler.hpp"
#include "utility.hpp"
#include "patch.hpp"
#include "interceptor.hpp"
#include "arguments.hpp"
#include "debug_registers.hpp"

typedef std::map<DWORD, debug_register_vector> thread_id_map_type;

namespace
{
	thread_id_map_type thread_id_map;
	boost::mutex thread_id_map_mutex;
}

void add_debug_register_manipulation_entry(DWORD thread_id, debug_register_vector const & data)
{
	boost::mutex::scoped_lock lock(thread_id_map_mutex);
	thread_id_map[thread_id] = data;
}

bool process_breakpoint(CONTEXT & thread_context)
{
	boost::mutex::scoped_lock lock(thread_id_map_mutex);
	DWORD thread_id = GetCurrentThreadId();
	thread_id_map_type::iterator iterator = thread_id_map.find(thread_id);
	if(iterator == thread_id_map.end())
		return false;

	debug_register_vector & data = iterator->second;

	thread_context.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;
	set_debug_registers(thread_context, data);

	thread_context.Eip++;

	return true;
}

void set_own_context(debug_register_vector const & data)
{
	add_debug_register_manipulation_entry(GetCurrentThreadId(), data);
	//write_line("set_own_context: " + ail::hex_string_32(reinterpret_cast<unsigned>(&set_own_context)));
	__asm
	{
		int 3
	}
}

LONG WINAPI vectored_exception_handler(PEXCEPTION_POINTERS ExceptionInfo)
{
	//nasty
	DWORD const DBG_PRINTEXCEPTION_C = 0x40010006;

	EXCEPTION_RECORD & exception_record = *(ExceptionInfo->ExceptionRecord);
	CONTEXT & thread_context = *(ExceptionInfo->ContextRecord);

	//write_line("Exception type " + ail::hex_string_32(exception_record.ExceptionCode) + " at " + ail::hex_string_32(thread_context.Eip) + " in thread " + ail::hex_string_32(GetCurrentThreadId()));

	switch(exception_record.ExceptionCode)
	{
	case EXCEPTION_BREAKPOINT:
		if(process_breakpoint(thread_context))
			return EXCEPTION_CONTINUE_EXECUTION;
		break;

	case EXCEPTION_SINGLE_STEP:
		perform_debug_register_check(thread_context);
		return EXCEPTION_CONTINUE_EXECUTION;

	case DBG_PRINTEXCEPTION_C:
		if(verbose)
			write_line("Debug message");
		break;
	}

	return EXCEPTION_CONTINUE_SEARCH;
}

bool install_exception_handler()
{
	if(!AddVectoredExceptionHandler(0, &vectored_exception_handler))
	{
		error("Failed to add vectored exception handler: " + ail::hex_string_32(GetLastError()));
		return false;
	}
	return true;
}
