#pragma once

#include "d2_data.hpp"

level_data * get_level_pointer(miscellaneous_act_data * act_pointer, unsigned level_number);
bool reveal_level(level_data * level_pointer);
bool reveal_act();
void draw_presets(room_data_type_2 * room_type_2_pointer);
