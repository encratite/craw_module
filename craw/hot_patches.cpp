#include <iostream>
#include <string>
#include <cstdio>
#include <boost/foreach.hpp>
#include <ail/string.hpp>
#include <ail/memory.hpp>
#include <ail/time.hpp>
#include <ail/file.hpp>
#include <ail/net.hpp>
#include <winsock2.h>
#include "patch.hpp"
#include "arguments.hpp"
#include "utility.hpp"
#include "debug_registers.hpp"
#include "d2_cdkey.hpp"
#include "hide.hpp"
#include "interceptor.hpp"
#include "keyboard.hpp"
#include "d2_functions.hpp"
#include "python.hpp"

HWND d2_window;

typedef HWND (WINAPI * FindWindow_type)(LPCTSTR lpClassName, LPCTSTR lpWindowName);
typedef HANDLE (WINAPI * CreateFile_type)(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
typedef HANDLE (WINAPI * CreateThread_type)(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);
typedef HMODULE (WINAPI * LoadLibrary_type)(LPCTSTR lpFileName);
typedef HWND (WINAPI * CreateWindowEx_type)(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
typedef int (WINAPI * recv_type)(SOCKET s, char *buf, int len, int flags);
typedef int (WINAPI * send_type)(SOCKET s, const char * buf, int len, int flags);
typedef SIZE_T (WINAPI * VirtualQuery_type)(LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength);
typedef ATOM (WINAPI * RegisterClass_type)(CONST WNDCLASS *lpWndClass);
typedef int (WINAPI * connect_type)(SOCKET s, const struct sockaddr * name, int namelen);

namespace
{
	FindWindow_type real_FindWindow;
	CreateFile_type real_CreateFile;
	CreateThread_type real_CreateThread;
	LoadLibrary_type real_LoadLibrary;
	CreateWindowEx_type real_CreateWindowEx;
	connect_type real_connect;
	recv_type real_recv;
	send_type real_send;
	VirtualQuery_type real_VirtualQuery;
	RegisterClass_type real_RegisterClass;

	WNDPROC d2_window_procedure;
	int bncs_socket = 0;
}

void send_bncs_data(std::string const & data)
{
	if(bncs_socket != 0)
		real_send(bncs_socket, data.c_str(), static_cast<int>(data.size()), 0);
}

HWND WINAPI patched_FindWindow(LPCTSTR lpClassName, LPCTSTR lpWindowName)
{
	if(std::string(lpClassName) == "Diablo II")
		return NULL;
	else
		return real_FindWindow(lpClassName, lpWindowName);
}

HANDLE WINAPI patched_CreateFile(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	std::string path(lpFileName);

	std::string file_name;
	std::size_t offset = path.rfind('\\');
	if(offset == std::string::npos)
	{
		offset = 0;
		file_name = path;
	}
	else
	{
		offset++;
		file_name = path.substr(offset);
	}

	std::string fixed_path;

	if(!bncache_directory.empty() && file_name == "bncache.dat")
	{
		std::string file_name = ail::number_to_string<DWORD>(GetCurrentProcessId()) + ".dat";
		fixed_path = ail::join_paths(bncache_directory, file_name);
	}
	else
		fixed_path = path;
	return real_CreateFile(fixed_path.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

HANDLE WINAPI patched_CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId)
{
	unsigned return_address;

	__asm
	{
		mov eax, [esp]
		mov return_address, eax
	}

	DWORD thread_id;
	dwCreationFlags |= CREATE_SUSPENDED;
	HANDLE output = real_CreateThread(lpThreadAttributes, dwStackSize, lpStartAddress, lpParameter, dwCreationFlags, &thread_id);
	if(verbose)
		write_line("New thread " + ail::hex_string_32(thread_id) + " at " + ail::hex_string_32(reinterpret_cast<unsigned>(lpStartAddress)) + " created by " + ail::hex_string_32(return_address));
	if(lpThreadId)
		*lpThreadId = thread_id;

	CONTEXT thread_context;
	thread_context.ContextFlags = CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;
	if(GetThreadContext(output, &thread_context))
	{
		unsigned thread_target = reinterpret_cast<unsigned>(lpStartAddress);
		perform_debug_register_trigger_check(output, thread_context, thread_target);
	}
	else
		write_line("Failed to retrieve thread context!");

	if(ResumeThread(output) == -1)
		write_line("Failed to resume thread!");

	return output;
}

HMODULE WINAPI patched_LoadLibrary(LPCTSTR lpFileName)
{
	HMODULE output = real_LoadLibrary(lpFileName);
	std::string file_name = lpFileName;
	if(verbose)
		write_line("Loaded DLL " + file_name + ": " + ail::hex_string_32(reinterpret_cast<unsigned>(output)));
	perform_dll_check();
	return output;
}

std::string get_d2_window_name()
{
	if(incremental_window_title)
	{
		unsigned i = 0;
		while(true)
		{
			std::string name = "D" + ail::number_to_string(i);
			if(!real_FindWindow(0, name.c_str()))
				return name;
			i++;
		}
	}
	else
		return window_title;
}

HWND WINAPI patched_CreateWindowEx(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	std::string name = lpWindowName;
	if(!window_title.empty() && name == "Diablo II")
	{
		name = get_d2_window_name();
		if(verbose)
			write_line("Using window name " + name);
	}
	HWND output = real_CreateWindowEx(dwExStyle, lpClassName, name.c_str(), dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	if(verbose)
		write_line("CreateWindowEx returned " + ail::hex_string_32(reinterpret_cast<unsigned>(output)));
	if(output == 0)
		write_line("CreateWindowEx failed with error number " + ail::hex_string_32(GetLastError()));
	d2_window = output;
	return output;
}

bool socket_recv(int socket, std::string & output)
{
	char buffer[2048];
	int result = real_recv(socket, buffer, static_cast<int>(sizeof(buffer)), 0);
	if(result == SOCKET_ERROR || result == 0)
	{
		write_line("Failed to receive data from the SOCKS server");
		return false;
	}

	output.assign(buffer, static_cast<std::size_t>(result));

	return true;
}

bool socket_send(int socket, std::string const & data)
{
	if(!real_send(socket, data.c_str(), static_cast<int>(data.size()), 0))
	{
		write_line("Failed to send data to SOCKS server");
		return false;
	}
	return true;
}

namespace
{
	ulong last_ip;
}

int WINAPI patched_connect(SOCKET s, const struct sockaddr * name, int namelen)
{
	sockaddr_in const & d2_server_address = *reinterpret_cast<sockaddr_in const *>(name);

	ulong d2_ip = ::ntohl(d2_server_address.sin_addr.s_addr);
	ushort d2_port = ::ntohs(d2_server_address.sin_port);

	if(use_socks)
	{
		write_line("Diablo II client is connecting to " + ail::convert_ipv4(d2_ip) + ":" + ail::number_to_string(d2_port) + " on socket " + ail::hex_string_32(s));

		if(namelen != sizeof(sockaddr_in))
		{
			write_line("connect: Invalid name length!");
			return SOCKET_ERROR;
		}

		u_long ioctl_argument = 0;
		if(::ioctlsocket(s, FIONBIO, &ioctl_argument) != 0)
		{
			write_line("Unable to make socket blocking");
			return SOCKET_ERROR;
		}

		ulong result = ::inet_addr(socks_server.c_str());
		ulong address_number;
		if(result == INADDR_NONE)
		{
			::hostent * host_entity = ::gethostbyname(socks_server.c_str());
			if(host_entity == 0)
			{
				write_line("Failed to perform DNS lookup for the SOCKS server");
				return SOCKET_ERROR;
			}
			address_number = *reinterpret_cast<ulong *>(host_entity->h_addr);
		}
		else
			address_number = result;

		sockaddr_in server_address;
		server_address.sin_family = AF_INET;
		server_address.sin_addr.s_addr = static_cast<u_long>(address_number);
		server_address.sin_port = ::htons(static_cast<u_short>(socks_port));

		write_line("Connecting to SOCKS server " + socks_server + ":" + ail::number_to_string(socks_port));

		int output = real_connect(s, reinterpret_cast<sockaddr *>(&server_address), static_cast<int>(sizeof(server_address)));
		if(output == SOCKET_ERROR)
		{
			write_line("Failed to connect the SOCKS server: " + ail::number_to_string(GetLastError()));
			return output;
		}

		write_line("Connected to SOCKS server");

		if(!socket_send(s, std::string("\x05\x01\x00", 3)))
			return SOCKET_ERROR;

		write_line("Waiting for login confirmation");
		
		std::string packet;
		if(!socket_recv(s, packet))
			return SOCKET_ERROR;

		write_line("Got login reply");

		if(packet != std::string("\x05\x00", 2))
		{
			write_line("Failed to log onto SOCKS server: " + ail::hex_string(packet));
			return SOCKET_ERROR;
		}

		ulong target_ip = d2_ip;
		if(::htonl(target_ip) == address_number)
		{
			target_ip = last_ip;
			write_line("Diablo II is attempting to connect directly to the SOCKS server, setting the IP to " + ail::convert_ipv4(target_ip));
		}

		std::string connect_packet("\x05\x01\x00\x01", 4);
		connect_packet += ail::big_endian_string(target_ip, 4);
		connect_packet += ail::big_endian_string(d2_port, 2);

		write_line("Sending connect packet " + ail::hex_string(connect_packet));

		if(!socket_send(s, connect_packet))
			return SOCKET_ERROR;

		write_line("Waiting for connection confirmation");

		if(!socket_recv(s, packet))
			return SOCKET_ERROR;

		write_line("Returned " + ail::hex_string(packet));

		ioctl_argument = 1;
		if(::ioctlsocket(s, FIONBIO, &ioctl_argument) != 0)
		{
			write_line("Unable to make socket nonblocking");
			return SOCKET_ERROR;
		}

		//if(packet != std::string("\x05\x00\x00\x01\x00\x00\x00\x00\x00\x00", 10))

		if(packet.size() < 2 || packet[1] != 0)
		{
			write_line("SOCKS connection failed");
			return SOCKET_ERROR;
		}

		last_ip = target_ip;

		write_line("Successfully established a connection through the SOCKS server");

		return 0;
	}
	else
		return real_connect(s, name, namelen);
}

int WINAPI patched_recv(SOCKET s, char * buf, int len, int flags)
{
	int output = real_recv(s, buf, len, flags);
	if(output != SOCKET_ERROR)
	{
		std::string data(buf, static_cast<std::size_t>(len));
		if(data.size() >= 2 && data.substr(0, 2) == "\xff\x25")
			bncs_socket = s;

		if(s == bncs_socket)
			python::perform_bncs_callback(data);
	}
	return output;
}

int WINAPI patched_send(SOCKET s, const char * buf, int len, int flags)
{
	std::string data(buf, len);
	if(use_custom_keys && data.size() > 64 && data.substr(0, 2) == "\xff\x51")
	{
		ulong client_token = ail::read_little_endian(data.c_str(), 4, 4);

		if(server_token_address == 0)
		{
			error("Unable to retrieve the server token");
			exit_process();
		}

		ulong server_token = ail::read_little_endian(reinterpret_cast<char *>(server_token_address), 4);

		std::string
			classic_hash,
			classic_public,
			expansion_hash,
			expansion_public;

		std::string original_packet = ail::hex_string(data);

		if(!hash_d2key(classic_key, client_token, server_token, classic_hash, classic_public))
		{
			error("Invalid Diablo II classic key!");
			exit_process();
		}

		if(!hash_d2key(expansion_key, client_token, server_token, expansion_hash, expansion_public))
		{
			error("Invalid Diablo II expansion key!");
			exit_process();
		}

		if(verbose)
			write_line("Replacing the CD keys in the outgoing packet");

		data.replace(32, 4, classic_public);
		data.replace(40, 20, classic_hash);

		data.replace(68, 4, expansion_public);
		data.replace(76, 20, expansion_hash);
		
		return real_send(s, data.c_str(), static_cast<int>(data.size()), flags);
	}
	else
		return real_send(s, buf, len, flags);
}

SIZE_T WINAPI patched_VirtualQuery(LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength)
{
	if(is_hidden_module(lpAddress))
	{
		//write_line("Warden tried to scan a hidden module at " + ail::hex_string_32(reinterpret_cast<unsigned>(lpAddress)));
		return 0;
	}
	return real_VirtualQuery(lpAddress, lpBuffer, dwLength);
}

LRESULT CALLBACK patched_window_procedure(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
		case WM_CHAR:
		{
			unsigned virtual_key = static_cast<unsigned>(wParam);
			process_key(virtual_key);
			break;
		}
	}
	return d2_window_procedure(hWnd, Msg, wParam, lParam);
}

ATOM WINAPI patched_RegisterClass(CONST WNDCLASS * lpWndClass)
{
	WNDCLASS & window_class = *const_cast<WNDCLASS *>(lpWndClass);
	d2_window_procedure = window_class.lpfnWndProc;
	window_class.lpfnWndProc = patched_window_procedure;
	if(verbose)
		write_line("Replaced the Diablo II window procedure with our own");
	return real_RegisterClass(lpWndClass);
}



bool apply_hot_patches()
{
	if(!patch_system_modules)
		return true;

	std::string const
		kernel = "kernel32.dll",
		user = "user32.dll",
		winsock = "wsock32.dll",
		winsock2 = "ws2_32.dll";

	hot_patch_entry patches[] =
	{
		hot_patch_entry(user, "FindWindowA", &patched_FindWindow, reinterpret_cast<void * &>(real_FindWindow)),
		hot_patch_entry(user, "CreateWindowExA", &patched_CreateWindowEx, reinterpret_cast<void * &>(real_CreateWindowEx)),
		hot_patch_entry(kernel, "CreateFileA", &patched_CreateFile, reinterpret_cast<void * &>(real_CreateFile)),
		hot_patch_entry(kernel, "CreateThread", &patched_CreateThread, reinterpret_cast<void * &>(real_CreateThread)),
		hot_patch_entry(kernel, "LoadLibraryA", &patched_LoadLibrary, reinterpret_cast<void * &>(real_LoadLibrary)),
		hot_patch_entry(winsock2, "connect", &patched_connect, reinterpret_cast<void * &>(real_connect)),
		hot_patch_entry(winsock, "recv", &patched_recv, reinterpret_cast<void * &>(real_recv)),
		hot_patch_entry(winsock2, "send", &patched_send, reinterpret_cast<void * &>(real_send)),
		hot_patch_entry(kernel, "VirtualQuery", &patched_VirtualQuery, reinterpret_cast<void * &>(real_VirtualQuery)),
		hot_patch_entry(user, "RegisterClassA", &patched_RegisterClass, reinterpret_cast<void * &>(real_RegisterClass)),
	};

	BOOST_FOREACH(hot_patch_entry & entry, patches)
	{
		if(!entry.enabled)
			continue;

		if(!hot_patch_function(entry.module, entry.procedure, entry.function, entry.real_function))
			return false;
	}

	return true;
}
