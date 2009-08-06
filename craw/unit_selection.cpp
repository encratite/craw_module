/*
Command line arguments: python E:\Code\Python\ida\ida.py D2Client.dll B:\D2\D2Client.lst sub_6FAD0010 unit_selection_test E:\Code\craw_module\source\craw\unit_selection.cpp
Time of generation: 2009-08-03 12:40:28
*/

#include <string>
#include <windows.h>

namespace
{
	//Initialisation variables

	char const * module_name = "D2Client.dll";
	unsigned image_base = 0x6FAB0000;
	unsigned module_base = 0;

	unsigned sub_6FACEAC0 = 0x6FACEAC0;
	unsigned sub_6FACF1E0 = 0x6FACF1E0;
	unsigned D2Common_10188 = 0x6FABC33A;
	unsigned sub_6FAB13D0 = 0x6FAB13D0;
	unsigned get_y_coordinate = 0x6FABC214;
	unsigned sub_6FACFE60 = 0x6FACFE60;
	unsigned D2Common_10706 = 0x6FABC20E;
	unsigned custom_exit = 0x6FAB326D;
	unsigned Fog_10024 = 0x6FABBF80;
	unsigned sub_6FACF7B0 = 0x6FACF7B0;
	unsigned sub_6FACF6B0 = 0x6FACF6B0;
	unsigned Fog_10265 = 0x6FABBF8C;
	unsigned sub_6FACF980 = 0x6FACF980;
	unsigned sub_6FACEE60 = 0x6FACEE60;
	unsigned D2Common_10670 = 0x6FABC226;
	unsigned sub_6FB67D50 = 0x6FB67D50;
	unsigned D2Common_10350 = 0x6FABC3D6;
	unsigned sub_6FACEDA0 = 0x6FACEDA0;
	unsigned sub_6FB42330 = 0x6FB42330;
	unsigned get_x_coordinate = 0x6FABC208;

}

void unit_selection_test_interrupt()
{
	__asm
	{
		int 3
	}
}

void unit_selection_test();

void __stdcall initialise_unit_selection_test()
{
	module_base = reinterpret_cast<unsigned>(GetModuleHandle(module_name));
	if(module_base == 0)
		unit_selection_test_interrupt();
	
	unsigned * call_addresses[] =
	{
		&sub_6FACEAC0,
		&sub_6FACF1E0,
		&D2Common_10188,
		&sub_6FAB13D0,
		&get_y_coordinate,
		&sub_6FACFE60,
		&D2Common_10706,
		&custom_exit,
		&Fog_10024,
		&sub_6FACF7B0,
		&sub_6FACF6B0,
		&Fog_10265,
		&sub_6FACF980,
		&sub_6FACEE60,
		&D2Common_10670,
		&sub_6FB67D50,
		&D2Common_10350,
		&sub_6FACEDA0,
		&sub_6FB42330,
		&get_x_coordinate
	};
	
	unsigned linking_offset = module_base - image_base;
	
	for(std::size_t i = 0; i < 20; i++)
	{
		unsigned & address = *call_addresses[i];
		address += linking_offset;
	}
	
	bool success = false;
	
	std::string const marker = "\x0f\x0b\x0f\x0b\x0f\x0b\x0f\x0b";
	
	char * data_pointer = reinterpret_cast<char *>(&unit_selection_test);
	while(true)
	{
		std::string current_string(data_pointer, marker.size());
		if(current_string == marker)
		{
			success = true;
			break;
		}
		data_pointer++;
	}
	
	if(!success)
		unit_selection_test_interrupt();
	
	data_pointer += marker.size();
	
	for(unsigned i = 0; i < 9; i++)
	{
		char * label_pointer = *reinterpret_cast<char **>(data_pointer + 1);
		unsigned * immediate_pointer = reinterpret_cast<unsigned *>(label_pointer - 4);
		DWORD old_protection;
		SIZE_T const patch_size = 4;
		if(!VirtualProtect(immediate_pointer, patch_size, PAGE_EXECUTE_READWRITE, &old_protection))
			unit_selection_test_interrupt();
		unsigned & address = *immediate_pointer;
		address += linking_offset;
		DWORD unused;
		if(!VirtualProtect(immediate_pointer, patch_size, old_protection, &unused))
			unit_selection_test_interrupt();
		data_pointer += 5;
	}
}

void __declspec(naked) unit_selection_test()
{
	__asm
	{
		//Initialisation code:
		
		cmp module_base, 0
		jnz is_already_initialised
		
		pushad
		call initialise_unit_selection_test
		popad
		
	is_already_initialised:
	
		//Actual code starts here:
		
		mov eax, ds:[06FBCC1D0h]
	linker_address_0:
		sub esp, 28h
		dec eax
		push ebp
		mov ebp, ds:[esp + 2Ch4]
		jz loc_6FAD004C
		dec eax
		jz loc_6FAD002F
		dec eax
		jnz loc_6FAD0060
		xor eax, eax
		pop ebp
		add esp, 28h
		retn 10h
	loc_6FAD002F:
		mov eax, ds:[06FB8C6E0h]
	linker_address_1:
		mov ecx, ds:[06FBB1638h]
	linker_address_2:
		cdq
		sub eax, edx
		sar eax, 1
		cmp ecx, eax
		jge loc_6FAD0060
	loc_6FAD0043:
		xor eax, eax
		pop ebp
		add esp, 28h
		retn 10h
	loc_6FAD004C:
		mov eax, ds:[06FB8C6E0h]
	linker_address_3:
		mov ecx, ds:[06FBB1638h]
	linker_address_4:
		cdq
		sub eax, edx
		sar eax, 1
		cmp ecx, eax
		jge loc_6FAD0043
	loc_6FAD0060:
		test ebp, ebp
		push ebx
		mov ebx, ds:[06FBCC3D0h]
	linker_address_5:
		jz loc_6FAD0087
		mov eax, ds:[ebp + 0C4h]
		shr eax, 15h
		and eax, 1
		mov ds:[esp + 30h4], eax
		jz loc_6FAD0087
		pop ebx
		xor eax, eax
		pop ebp
		add esp, 28h
		retn 10h
	loc_6FAD0087:
		push esi
		push edi
		push ebx
		mov eax, ebp
		call sub_6FACF980
		test eax, eax
		jnz loc_6FAD00B1
		mov edi, ebp
		mov eax, ebx
		call sub_6FACF1E0
		test eax, eax
		jnz loc_6FAD00B1
		mov esi, ebx
		call sub_6FACF7B0
		test eax, eax
		jz loc_6FAD0239
	loc_6FAD00B1:
		mov esi, ebp
		mov edi, ebx
		mov ds:[esp + 38h4], 0
		call sub_6FACF6B0
		test eax, eax
		jz loc_6FAD00F7
		test ebp, ebp
		jz loc_6FAD0239
		mov edx, ds:[ebp + 0C4h]
		mov ecx, edx
		shr ecx, 1
		and ecx, 1
		mov ds:[esp + 38h - 18h], ecx
		jnz loc_6FAD0110
		cmp dword ptr ds:[ebp + 0], 1
		jnz loc_6FAD0118
		mov eax, ebp
		call sub_6FACEAC0
		test eax, eax
		jz loc_6FAD0118
		shr edx, 2
		jmp loc_6FAD0107
	loc_6FAD00F7:
		test ebp, ebp
		jz loc_6FAD0239
		mov edx, ds:[ebp + 0C4h]
		shr edx, 1
	loc_6FAD0107:
		and edx, 1
		mov ds:[esp + 38h - 18h], edx
		jz loc_6FAD0118
	loc_6FAD0110:
		mov ds:[esp + 38h4], 1
	loc_6FAD0118:
		mov eax, ds:[ebp + 0C8h]
		shr eax, 7
		and eax, 1
		mov ds:[esp + 38h - 18h], eax
		jz loc_6FAD0239
		mov eax, ds:[esp + 38h4]
		test eax, eax
		jz loc_6FAD0239
		mov eax, ds:[esp + 38h10h]
		test eax, eax
		jz loc_6FAD0467
		push ebp
		call D2Common_10706
		mov ecx, ds:[ebp + 4]
		mov edx, ds:[ebp + 0]
		push 6EDh
		push 06FB86C54h
	linker_address_6:
		mov ds:[esp + 40h - 1Ch], eax
		lea eax, ds:[esp + 40h - 1Ch]
		push eax
		mov ds:[esp + 44h - 28h], ecx
		lea ecx, ds:[esp + 44h - 28h]
		mov ds:[esp + 44h - 24h], edx
		push ecx
		lea edx, ds:[esp + 48h - 24h]
		push edx
		push ebp
		call D2Common_10350
		xor ebx, ebx
		push ebp
		mov ds:[esp + 3Ch4], ebx
		call get_x_coordinate
		push ebp
		mov esi, eax
		call get_y_coordinate
		mov edi, eax
		mov eax, ds:[esp + 38h - 24h]
		cmp eax, 5
		ja loc_6FAD028C
		jmp ds:off_6FAD0478[eax * 4]
	loc_6FAD01A7:
		mov eax, ds:[esp + 38h - 28h]
		call sub_6FAB13D0
		mov cl, ds:[eax + 4]
		mov edx, gdwBitMasks
		test ds:[edx], cl
		jz loc_6FAD0216
		movsx edx, word ptr ds:[eax + 110h]
		movsx ecx, word ptr ds:[eax + 10Eh]
		add edx, esi
		movsx esi, word ptr ds:[eax + 112h]
		movsx eax, word ptr ds:[eax + 114h]
		add ecx, edi
		add eax, ecx
		mov ds:[esp + 38h - 10h.top], ecx
		mov ecx, ds:[esp + 38h8.y]
		add esi, edx
		mov ds:[esp + 38h - 10h.left], edx
		mov edx, ds:[esp + 38h8.x]
		push ecx
		mov ds:[esp + 3Ch - 10h.bottom], eax
		push edx
		lea eax, ds:[esp + 40h - 10h]
		push eax
		mov ds:[esp + 44h - 10h.right], esi
		call ds:PtInRect
		pop edi
		neg eax
		pop esi
		sbb eax, eax
		pop ebx
		neg eax
		pop ebp
		add esp, 28h
		retn 10h
	loc_6FAD0216:
		mov ecx, ds:[esp + 38h8.y]
		mov edx, ds:[esp + 38h8.x]
		push ecx
		push edx
		jmp loc_6FAD022C
	loc_6FAD0222:
		mov eax, ds:[esp + 38h8.y]
		mov ecx, ds:[esp + 38h8.x]
		push eax
		push ecx
	loc_6FAD022C:
		push edi
		push esi
		mov eax, ebp
		call sub_6FB42330
		test eax, eax
		jnz loc_6FAD0288
	loc_6FAD0239:
		pop edi
		pop esi
		pop ebx
		xor eax, eax
		pop ebp
		add esp, 28h
		retn 10h
	loc_6FAD0245:
		mov edx, ds:[esp + 38h - 28h]
		push edx
		call D2Common_10670
		mov ecx, ds:[esp + 38h8.x]
		mov ds:[esp + 38h4], eax
		mov eax, ds:[esp + 38h8.y]
		push eax
		push ecx
		push edi
		push esi
		mov eax, ebp
		call sub_6FB42330
		test eax, eax
		jnz loc_6FAD0284
		mov edx, ds:[esp + 38h4]
		mov al, ds:[edx + 13Ah]
		test al, al
		jnz loc_6FAD0284
		mov eax, edx
		mov cl, ds:[eax + 19Dh]
		test cl, cl
		jz loc_6FAD0239
	loc_6FAD0284:
		mov ebx, ds:[esp + 38h4]
	loc_6FAD0288:
		mov eax, ds:[esp + 38h - 24h]
	loc_6FAD028C:
		xor esi, esi
	loc_6FAD028E:
		xor edi, edi
		sub eax, esi
		jz loc_6FAD0386
		dec eax
		jz loc_6FAD0370
		dec eax
		jnz loc_6FAD038B
		cmp ebx, esi
		jnz loc_6FAD0305
		push 741h
		call Fog_10265
		push eax
		push 06FB80323h
	linker_address_7:
		call Fog_10024
		add esp, 0Ch
		push 0FFFFFFFFh
		call custom_exit
	loc_6FAD02C9:
		cmp dword ptr ds:[ebp + 10h], 3
		jnz loc_6FAD0239
		mov ecx, ds:[06FBB2B8Ch]
	linker_address_8:
		xor esi, esi
		cmp ecx, esi
		jz loc_6FAD028E
		pop edi
		pop esi
		pop ebx
		xor eax, eax
		pop ebp
		add esp, 28h
		retn 10h
	loc_6FAD02EB:
		mov ecx, ds:[esp + 38h8.y]
		mov ebx, ds:[esp + 38h8.x]
		push ecx
		mov eax, ebp
		call sub_6FACEE60
		pop edi
		pop esi
		pop ebx
		pop ebp
		add esp, 28h
		retn 10h
	loc_6FAD0305:
		mov al, ds:[ebx + 13Ah]
		test al, al
		jz loc_6FAD033F
		mov eax, ds:[ebp + 10h]
		cmp eax, 2
		jz loc_6FAD0321
		cmp eax, 4
		jz loc_6FAD0321
		cmp eax, 5
		jnz loc_6FAD0367
	loc_6FAD0321:
		mov edx, ds:[esp + 38h8.y]
		mov ebx, ds:[esp + 38h8.x]
		mov edi, ds:[esp + 38h4]
		push edx
		mov eax, ebp
		call sub_6FACEDA0
		pop edi
		pop esi
		pop ebx
		pop ebp
		add esp, 28h
		retn 10h
	loc_6FAD033F:
		mov al, ds:[ebx + 19Dh]
		test al, al
		jz loc_6FAD0367
		mov eax, ds:[esp + 38h8.y]
		mov ebx, ds:[esp + 38h8.x]
		mov edi, ds:[esp + 38h4]
		push eax
		mov eax, ebp
		call sub_6FACEDA0
		pop edi
		pop esi
		pop ebx
		pop ebp
		add esp, 28h
		retn 10h
	loc_6FAD0367:
		cmp byte ptr ds:[ebx + 161h], 1
		jmp loc_6FAD0384
	loc_6FAD0370:
		mov eax, ds:[esp + 38h - 28h]
		call sub_6FAB13D0
		cmp eax, esi
		jz loc_6FAD038B
		cmp byte ptr ds:[eax + 0ECh], 1
	loc_6FAD0384:
		jbe loc_6FAD038B
	loc_6FAD0386:
		mov edi, 1
	loc_6FAD038B:
		lea ecx, ds:[esp + 38h - 14h]
		push ecx
		lea edx, ds:[esp + 3Ch - 20h]
		push edx
		push ebp
		call D2Common_10188
		cmp ds:[ebp + 0], esi
		mov ds:[esp + 38h - 10h.left], esi
		mov ds:[esp + 38h - 10h.top], esi
		mov ds:[esp + 38h - 10h.right], esi
		mov ds:[esp + 38h - 10h.bottom], esi
		jnz loc_6FAD03C0
		cmp dword ptr ds:[ebp + 4], 4
		jnz loc_6FAD03C0
		cmp dword ptr ds:[ebp + 10h], 12h
		jz loc_6FAD0467
	loc_6FAD03C0:
		cmp edi, esi
		jz loc_6FAD0419
	loc_6FAD03C4:
		mov ecx, ds:[esp + 38h - 20h]
		lea eax, ds:[esp + 38h - 10h]
		push eax
		push ecx
		mov eax, esi
		mov ecx, ebp
		call sub_6FACFE60
		test eax, eax
		jz loc_6FAD0407
		push esi
		mov eax, ebp
		call sub_6FB67D50
		test eax, eax
		jz loc_6FAD0407
		mov eax, ds:[esp + 38h8.x]
		cmp eax, ds:[esp + 38h - 10h.left]
		jl loc_6FAD0407
		cmp eax, ds:[esp + 38h - 10h.right]
		jg loc_6FAD0407
		mov eax, ds:[esp + 38h8.y]
		cmp eax, ds:[esp + 38h - 10h.bottom]
		jl loc_6FAD0407
		cmp eax, ds:[esp + 38h - 10h.top]
		jle loc_6FAD0467
	loc_6FAD0407:
		inc esi
		cmp esi, 10h
		jb loc_6FAD03C4
		pop edi
		pop esi
		pop ebx
		xor eax, eax
		pop ebp
		add esp, 28h
		retn 10h
	loc_6FAD0419:
		mov eax, ds:[esp + 38h - 20h]
		lea edx, ds:[esp + 38h - 10h]
		push edx
		push eax
		mov eax, 1
		mov ecx, ebp
		call sub_6FACFE60
		test eax, eax
		jz loc_6FAD0239
		mov eax, ds:[esp + 38h8.x]
		cmp eax, ds:[esp + 38h - 10h.left]
		jl loc_6FAD0239
		cmp eax, ds:[esp + 38h - 10h.right]
		jg loc_6FAD0239
		mov eax, ds:[esp + 38h8.y]
		cmp eax, ds:[esp + 38h - 10h.bottom]
		jl loc_6FAD0239
		cmp eax, ds:[esp + 38h - 10h.top]
		jg loc_6FAD0239
	loc_6FAD0467:
		pop edi
		pop esi
		pop ebx
		mov eax, 1
		pop ebp
		add esp, 28h
		retn 10h

		//Instruction address table hack:

		ud2
		ud2
		ud2
		ud2

		push linker_address_0
		push linker_address_1
		push linker_address_2
		push linker_address_3
		push linker_address_4
		push linker_address_5
		push linker_address_6
		push linker_address_7
		push linker_address_8
	}
}

