#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <cstring>
#include <ail/file.hpp>
#include <ail/random.hpp>
#include <ail/string.hpp>
#include <boost/foreach.hpp>
#include "hide.hpp"
#include "arguments.hpp"
#include "utility.hpp"

namespace
{
	std::vector<hidden_module> hidden_modules;
}

hidden_module::hidden_module()
{
}

hidden_module::hidden_module(void * address, std::size_t size):
	start_address(reinterpret_cast<unsigned>(address)),
		size(static_cast<unsigned>(size))
{
}

bool hidden_module::operator==(void const * address) const
{
	unsigned target_address = reinterpret_cast<unsigned>(address);
	//write_line("target_address: " + ail::hex_string_32(target_address) + " start_address: " + ail::hex_string_32(start_address) + ", size: " + ail::hex_string_32(size));
	return target_address >= start_address && target_address <= start_address + size;
}

//originally by Darawk, slightly modified

void unlink_entry(LIST_ENTRY & entry)
{
	entry.Blink->Flink = entry.Flink;
	entry.Flink->Blink = entry.Blink;
}

bool hide_module(void * module_base)
{
	PROCESS_MODULE_INFO * process_module_pointer;
	MODULE_INFO_NODE * module;

	_asm
	{
		mov eax, fs:[18h]
		mov eax, [eax + 30h]
		mov eax, [eax + 0Ch]
		mov process_module_pointer, eax
	}

    module = (MODULE_INFO_NODE *)(process_module_pointer->LoadOrder.Flink);
	
	while(true)
	{
		HMODULE base_address = module->baseAddress;
		if(base_address == 0)
			return false;
		if(module->baseAddress == module_base)
			break;
		module = (MODULE_INFO_NODE *)(module->LoadOrder.Flink);
	}

	if(!module->baseAddress)
		return false;

	unlink_entry(module->LoadOrder);
	unlink_entry(module->InitOrder);
	unlink_entry(module->MemoryOrder);
	unlink_entry(module->HashTable);

	std::memset(module->fullPath.Buffer, 0, module->fullPath.Length);
	std::memset(module, 0, sizeof(MODULE_INFO_NODE));

	return true;
}

bool destroy_pe_header(void * module_base)
{
	DWORD
		old_protection,
		unused;
	if(!VirtualProtect(module_base, page_size, PAGE_READWRITE, &old_protection))
	{
		error("Failed to make PE header writable");
		return false;
	}

	//std::memset(module_base, 0, page_size);

	uchar * data_pointer = reinterpret_cast<uchar *>(module_base);
	std::size_t magic_end = 2;

	//to avoid the accidental generation of "MZ"
	for(std::size_t i = 0; i < magic_end; i++)
		data_pointer[i] = ail::random_integer(0x80, 0xff);

	for(std::size_t i = magic_end; i < page_size; i++)
		data_pointer[i] = ail::random_integer(0, 0xff);

	if(!VirtualProtect(module_base, page_size, old_protection, &unused))
	{
		error("Failed to restore PE header protection");
		return false;
	}
	return true;
}

bool hide_module(std::string const & name)
{
	HMODULE module_handle = GetModuleHandle(name.c_str());
	if(module_handle == 0)
	{
		error("Unable to retrieve the handle of module " + name);
		return false;
	}

	void * address = reinterpret_cast<void *>(module_handle);
	bool success = hide_module(address) && destroy_pe_header(address);
	if(success)
	{
		MEMORY_BASIC_INFORMATION memory_basic_information;
		std::size_t size = static_cast<std::size_t>(VirtualQuery(address, &memory_basic_information, sizeof(MEMORY_BASIC_INFORMATION)));
		if(size == 0)
		{
			error("Unable to query the size of the allocation of module " + name);
			return false;
		}
		hidden_modules.push_back(hidden_module(address, size));
	}

	return success;
}

bool hide_modules()
{
	if(hide_modules_file.empty())
		return true;

	string_vector lines;
	if(!ail::read_lines(hide_modules_file, lines))
	{
		error("Unable to read the list of modules to hide: " + hide_modules_file);
		return false;
	}

	BOOST_FOREACH(std::string const & line, lines)
	{
		if(line.empty())
			continue;

		if(verbose)
			write_line("Hiding " + line);

		if(!hide_module(line))
			return false;
	}

	return true;
}

bool is_hidden_module(void const * address)
{
	return std::find(hidden_modules.begin(), hidden_modules.end(), address) != hidden_modules.end();
}
