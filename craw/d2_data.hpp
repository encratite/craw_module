#pragma once

#include <ail/types.hpp>

enum
{
	difficulty_count = 3
};

enum
{
	difficulty_normal,
	difficulty_nightmare,
	difficulty_hell
};

struct unit
{
	unsigned type;
	unsigned table_index;
	unsigned unknown0;
	unsigned id;
	unsigned mode;
	void * unit_data;
	unsigned act;
	void * act_data;
	unsigned seed[2];
	unsigned unknown1;
	void * path_data;
};

struct monster_data
{
	char unknown[22];
	uchar flags;

	bool is_minion();
	bool is_champion();
	bool is_boss();

	bool get_flag(unsigned offset);
};

struct path
{
	short offset_x;
	short position_x;
	short offset_y;
	short position_y;
};

struct treasure_class_entry
{
	ushort treasure_class[4];
};

struct monster_statistics
{
	char unused0[0x0c];

	unsigned flags;

	char unused1[0x86 - 0x10];

	treasure_class_entry treasure_classes[difficulty_count];

	char unused2[0xaa - 0x9e];

	ushort level[difficulty_count];
	ushort min_hp[difficulty_count];
	ushort max_hp[difficulty_count];

	char unused3[0xd4 - 0xbc];

	ushort experience[difficulty_count];

	char unused4[0x144 - 0xda];

	ushort damage_resistance[difficulty_count];
	ushort magic_resistance[difficulty_count];

	ushort fire_resistance[difficulty_count];
	ushort lightning_resistance[difficulty_count];
	ushort cold_resistance[difficulty_count];
	ushort poison_resistance[difficulty_count];

	char unused5[0x1a8 - 0x168];

	bool is_npc();
};

struct roster_unit
{
	char name[16];
	unsigned unit_id;
	unsigned life;
	unsigned unknown1;
	unsigned class_id;
	ushort level;
	ushort party_id;
	unsigned level_id;
	unsigned position_x;
	unsigned position_y;
	unsigned party_flags;
	uchar * unknown5;
	unsigned unknown6[11];
	ushort unknown7;
	char szName2[16];
	ushort unknown8;
	unsigned unknown9[2];
	roster_unit * next_roster;
};

struct level_data;
struct room_data_type_2;

struct miscellaneous_act_data
{
	unsigned unknown1;
	void * act_data;
	unsigned unknown2[282];
	level_data * first_level;
};

struct preset_unit
{
	unsigned table_index;
	unsigned unknown1[2];
	unsigned position_x;
	unsigned unknown2;
	unsigned position_y;
	preset_unit * next;
	unsigned type;
};

struct level_data
{
	unsigned unknown1[21];
	unsigned seed[2];
	level_data * next_level;
	unsigned unknown2;
	miscellaneous_act_data * act_pointer;
	unsigned unknown3;
	unsigned position_x;
	unsigned position_y;
	unsigned size_x;
	unsigned size_y;
	unsigned unknown4[6];
	unsigned level_number;
	unsigned unknown5[97];
	room_data_type_2 * first_room;
};

struct room_data_type_1
{
	room_data_type_1 ** near_rooms;
	void * act_data;
	unsigned unknown1;
	unsigned seed[2];
	unsigned unknown2;
	unsigned global_position_x;
	unsigned global_position_y;
	unsigned global_size_x;
	unsigned global_size_y;
	unsigned room_position_x;
	unsigned room_position_y;
	unsigned room_size_x;
	unsigned room_size_y;
	room_data_type_1 * next_room;
	int unknown3;
	unit * first_unit;
	unsigned unknown4[3];
	void * collision_map_data;
	unsigned unknown5[7];
	room_data_type_2 * room_2;
	unsigned unknown6[3];
};

struct room_data_type_2
{
	level_data * level;
	unsigned unknown1;
	unsigned near_room_count;
	void * room_tile_data;
	room_data_type_2 ** near_type_2_rooms;
	unsigned unknown2[5];
	room_data_type_2 * previous_room_2;
	unsigned position_x;
	unsigned position_y;
	unsigned size_x;
	unsigned size_y;
	unsigned unknown3[34];
	preset_unit * preset;
	unsigned unknown4[3];
	room_data_type_2 * other_room_2;
	room_data_type_1 * room_1;
	unsigned seed[2];
	unsigned unknown5;
	room_data_type_2 * next_room_2;
};
