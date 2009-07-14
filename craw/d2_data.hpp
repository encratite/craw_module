#pragma once

#include <ail/types.hpp>

//Cthulhon etc

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

struct act_data;
struct path_data;

struct unit
{
	unsigned type;
	unsigned table_index;
	unsigned unknown0;
	unsigned id;
	unsigned mode;
	void * unit_data;
	unsigned act;
	act_data * act_data_pointer;
	unsigned seed[2];
	unsigned unknown1;
	path_data * path_data_pointer;
};

struct monster_data
{
	void * unknown1;
	uchar unknown_byte_flags[5];
	uchar unknown2[3];
	unsigned unknown3;
	unsigned unknown4;

	ushort unused_value;
	uchar flags;
	uchar unknown5;
	unsigned unknown6;
	
	uchar special_abilities[10];

	/*
	char unknown1[22];
	uchar flags;
	ushort unknown2;
	unsigned unknown3;
	uchar special_abilities[9];
	uchar unknown4;
	ushort unique_number;
	unsigned unknown5;
	wchar_t name[28];
	*/

	bool is_minion();
	bool is_champion();
	bool is_boss();

	bool get_flag(unsigned offset);
};

struct room_data_type_1;

struct path_data
{
	ushort offset_x;
	ushort position_x;
	ushort offset_y;
	ushort position_y;
	unsigned unknown1[2];
	ushort target_x;
	ushort target_y;
	unsigned unknown2[2];
	room_data_type_1 * room_1;
	room_data_type_1 * unknown_room;
	unsigned unknown3[3];
	unit * unit_pointer;
	unsigned flags;
	unsigned unknown4;
	unsigned path_type;
	unsigned previous_path_type;
	unsigned unit_size;
	unsigned unknown5[4];
	unit * target_unit;
	unsigned target_type;
	unsigned target_id;
	uchar direction;
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
	char name2[16];
	ushort unknown8;
	unsigned unknown9[2];
	roster_unit * next_roster;
};

struct level_data;
struct room_data_type_1;
struct room_data_type_2;
struct miscellaneous_act_data;

struct act_data
{
	unsigned unknown1[13];
	room_data_type_1 * room_1;
	miscellaneous_act_data * miscellaneous_act_data_pointer;
	unsigned unknown2;
	unsigned act_number;
};

struct miscellaneous_act_data
{
	unsigned unknown1;
	act_data * act_data_pointer;
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

struct automap_cell
{
	unsigned saved;
	ushort cell_number;
	ushort x;
	ushort y;
	ushort weight;
	automap_cell * less;
	automap_cell * more;
};

struct automap_layer
{
	unsigned layer_number;
	unsigned saved;
	automap_cell * floors;
	automap_cell * walls;
	automap_cell * objects;
	automap_cell * extras;
	automap_layer * next_layer;
};

struct automap_layer_type_2
{
	unsigned unknown1[2];
	unsigned layer_number;
};

struct object_table_entry
{
	char name[0x40];
	wchar_t unicode_name[0x40];
	uchar unknown1[4];
	uchar selectable;
	uchar unknown2[0x87];
	uchar orientation;
	uchar unknown2b[0x19];
	uchar sub_class;
	uchar unknown3[0x11];
	uchar parameter;
	uchar unknown4[0x39];
	uchar populate;
	uchar operate;
	uchar unknown5[8];
	unsigned automap;
};
