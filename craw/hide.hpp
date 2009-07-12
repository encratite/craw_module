#pragma once

#include <windows.h>

struct hidden_module
{
	unsigned
		start_address,
		size;

	hidden_module();
	hidden_module(void * address, std::size_t size);

	bool operator==(void const * address) const;
};

//by Darawk

struct UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR Buffer;
};

struct MODULE_INFO_NODE
{
	LIST_ENTRY LoadOrder;
	LIST_ENTRY InitOrder;
	LIST_ENTRY MemoryOrder;
	HMODULE baseAddress;
	unsigned long entryPoint;
	unsigned int size;
	UNICODE_STRING fullPath;
	UNICODE_STRING name;
	unsigned long flags;
	unsigned short LoadCount;
	unsigned short TlsIndex;
	LIST_ENTRY HashTable;
	unsigned long timestamp;
};

struct PROCESS_MODULE_INFO
{
	unsigned int size;
	unsigned int initialized;
	HANDLE SsHandle;
	LIST_ENTRY LoadOrder;
	LIST_ENTRY InitOrder;
	LIST_ENTRY MemoryOrder;
};

bool hide_module(void * module_base);
bool hide_modules();
bool is_hidden_module(void const * address);
