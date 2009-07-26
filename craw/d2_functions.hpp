#pragma once

#include <string>
#include <vector>
#include <ail/types.hpp>
#include "d2_data.hpp"

typedef std::vector<roster_unit> roster_vector;

void initialise_d2win_addresses(unsigned base);
void initialise_d2gfx_addresses(unsigned base);
void initialise_d2common_addresses(unsigned base);
void initialise_d2client_addresses(unsigned base);
void initialise_d2net_addresses(unsigned base);
void initialise_bnclient_addresses(unsigned base);

typedef unsigned (__fastcall * set_font_size_type)(unsigned new_size);
typedef void (__fastcall * draw_text_type)(wchar_t const * text, int x, int y, unsigned colour, int unknown);
typedef unsigned (__fastcall * get_text_width_type)(wchar_t const * text, unsigned * width, unsigned * file_number);
typedef void (__stdcall * draw_line_type)(int start_x, int start_y, int end_x, int end_y, unsigned colour, unsigned unknown);
typedef unsigned (__stdcall * get_unit_stat_type)(unit * unit_pointer, unsigned stat, unsigned stat2);
typedef unit * (__stdcall * get_player_unit_type)();
typedef uchar (__stdcall * get_difficulty_type)();
typedef level_data * (__fastcall * get_level_type)(miscellaneous_act_data * act, unsigned level_number);
typedef automap_layer_type_2 * (__fastcall * get_layer_type)(unsigned level_number);
typedef void (__stdcall * add_room_data_tye)(act_data * act, unsigned level_id, unsigned x, unsigned y, room_data_type_1 * room);
typedef void (__stdcall * reveal_automap_room_type)(room_data_type_1 * room, unsigned clip_flag, void * layer);
typedef void (__stdcall * remove_room_data_type)(void * act, unsigned level_id, unsigned x, unsigned y, room_data_type_1 * room);
typedef void (__stdcall * initialise_level_type)(level_data * level);
typedef object_table_entry * (__stdcall * get_object_table_entry_type)(unsigned object_number);
typedef automap_cell * (__fastcall * new_automap_cell_type)();
typedef void (__fastcall * add_automap_cell_type)(automap_cell * cell, automap_cell ** node);
typedef void (__stdcall * send_packet_type)(std::size_t size, unsigned flags, char const * packet);
typedef void (__stdcall * leave_game_type)();
typedef unit * (__fastcall * get_unit_pointer_type)(unsigned id, unsigned type);
typedef unit * (__stdcall * get_inventory_item_type)(void * inventory);
typedef unit * (__stdcall * get_next_inventory_item_type)(unit * item);
typedef item_text * (__stdcall * get_item_text_type)(unsigned item_number);
typedef void (__stdcall * receive_packet_type)(char const * packet, std::size_t size);
typedef bool (__stdcall * get_item_name_type)(unit * item, wchar_t * buffer, std::size_t buffer_size);
typedef void (__stdcall * map_to_screen_coordinates_type)(int * x, int * y);
typedef void (__stdcall * click_map_type)(unsigned flags, int x, int y, unsigned type);

extern set_font_size_type d2_set_font_size;
extern draw_text_type d2_draw_text;
extern get_text_width_type d2_get_text_width;
extern draw_line_type d2_draw_line;
extern get_unit_stat_type d2_get_unit_stat;
extern get_player_unit_type d2_get_player_unit;
extern get_difficulty_type d2_get_difficulty;
extern get_level_type d2_get_level;
extern get_layer_type d2_get_layer;
extern add_room_data_tye d2_add_room_data;
extern reveal_automap_room_type d2_reveal_automap_room;
extern remove_room_data_type d2_remove_room_data;
extern initialise_level_type d2_initialise_level;
extern get_object_table_entry_type d2_get_object_table_entry;
extern new_automap_cell_type d2_new_automap_cell;
extern add_automap_cell_type d2_add_automap_cell;
extern send_packet_type d2_send_packet;
extern leave_game_type d2_leave_game;
extern get_unit_pointer_type d2_get_unit_pointer;
extern get_inventory_item_type d2_get_inventory_item;
extern get_next_inventory_item_type d2_get_next_inventory_item;
extern get_item_text_type d2_get_item_text;
extern receive_packet_type d2_receive_packet;
extern get_item_name_type d2_get_item_name;
extern map_to_screen_coordinates_type d2_map_to_screen_coordinates;
extern click_map_type d2_click_map;

extern unsigned light_handler_address;

extern unsigned automap_handler_address;
extern unsigned automap_loop_address;

extern unsigned server_token_address;

extern unsigned
	add_unit_address1,
	add_unit_address2;

extern unsigned
	item_handler_call_address;

extern unsigned
	mouse_x_address,
	mouse_y_address;

void draw_text(std::string const & text, int x, int y, unsigned colour, bool centered);
void __stdcall draw_box(int x, int y, unsigned colour);
bool get_life(unsigned & current_life, unsigned & maximum_life);
monster_statistics & get_monster_statistics(std::size_t index);
roster_unit * get_player_roster(unsigned player_id);
unit * get_player();
automap_layer * get_automap_layer();
automap_layer_type_2 * initialise_automap_layer(unsigned level_number);
automap_cell ** get_layer_objects_pointer();
bool get_player_level_number(unsigned & output);
bool get_player_id(unsigned & output);
wchar_t * get_unit_name(unit * unit_pointer);
bool get_name_by_id(unsigned id, std::string & output);
bool get_non_empty_tp_tome_id(unsigned & output);
bool player_is_in_game();
roster_vector get_roster_units();
bool get_item_name(unit * input, std::string & name, std::string & special_name);
void move_click(int x, int y);
