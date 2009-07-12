#include <ail/string.hpp>
#include "utility.hpp"
#include "reveal_map.hpp"
#include "d2_functions.hpp"

//from mMap, with a few changes

level_data * get_level_pointer(miscellaneous_act_data * act_pointer, unsigned level_number)
{
	if(!act_pointer || level_number < 0)
		return false;

	for(level_data * level_pointer = act_pointer->first_level; level_pointer; level_pointer = level_pointer->next_level)
	{
		if(!level_pointer)
			continue;

		if(level_pointer->level_number == level_number && level_pointer->size_x > 0)
			return level_pointer;
	}

	return d2_get_level(act_pointer, level_number);
}

bool reveal_level(level_data * level_pointer)
{
	if(!level_pointer)
		return false;

	if(!initialise_automap_layer(level_pointer->level_number))
		return false;

	unit * unit_pointer = d2_get_player_unit();
	if(!unit_pointer)
		return false;

	for(room_data_type_2 * room_type_2_pointer = level_pointer->first_room; room_type_2_pointer; room_type_2_pointer = room_type_2_pointer->other_room_2)
	{
		write_line("reveal_level room_type_2_pointer == " + ail::hex_string_32(reinterpret_cast<unsigned>(room_type_2_pointer)));

		bool room_data_has_been_added = false;

		if(!room_type_2_pointer->room_1)
		{
			write_line("d2_add_room_data");
			d2_add_room_data(level_pointer->act_pointer->act_data_pointer, level_pointer->level_number, room_type_2_pointer->position_x, room_type_2_pointer->position_y, unit_pointer->path_data_pointer->room_1);
			room_data_has_been_added = true;
		}

		if(!room_type_2_pointer->room_1)
			continue;
	
		//watch out for the automap layer pointer
		write_line("d2_reveal_automap_room");
		d2_reveal_automap_room(room_type_2_pointer->room_1, 1, get_automap_layer());

		draw_presets(room_type_2_pointer);

		if(room_data_has_been_added)
		{
			write_line("d2_remove_room_data");
			d2_remove_room_data(level_pointer->act_pointer->act_data_pointer, level_pointer->level_number, room_type_2_pointer->position_x, room_type_2_pointer->position_y, unit_pointer->path_data_pointer->room_1);
		}
	}

	initialise_automap_layer(unit_pointer->path_data_pointer->room_1->room_2->level->level_number);

	return true;
}

bool reveal_act()
{
	unit * unit_pointer = d2_get_player_unit();
	if(!unit_pointer)
		return false;

	//should be 133, not 132
	unsigned town_level_numbers[] = {1, 40, 75, 103, 109, 132 + 1};
	
	for(unsigned level_number = town_level_numbers[unit_pointer->act] + 1; level_number < town_level_numbers[unit_pointer->act + 1]; level_number++)
	{
		write_line("Processing level " + ail::hex_string_32(level_number));

		level_data * level_pointer = get_level_pointer(unit_pointer->act_data_pointer->miscellaneous_act_data_pointer, level_number);

		if(!level_pointer)
			continue;

		if(!level_pointer->first_room)
		{
			d2_initialise_level(level_pointer);
			continue;
		}

		write_line("Revealing the level");

		reveal_level(level_pointer);
	}

	return true;
}

void draw_presets(room_data_type_2 * room_type_2_pointer)
{
	for(preset_unit * preset_unit_pointer = room_type_2_pointer->preset; preset_unit_pointer; preset_unit_pointer = preset_unit_pointer->next)
	{
		int cell = -1;

		//Special NPCs.
		if(preset_unit_pointer->type == 1)
		{
			//Izual
			if(preset_unit_pointer->table_index == 256)
				cell = 300;

			//Hephasto
			else if(preset_unit_pointer->table_index == 745)
				cell = 745;
		}

		//Objects on the map
		else if(preset_unit_pointer->type == 2)
		{
			
			//Special Units that require special checking

			//Chest
			if(preset_unit_pointer->table_index == 397) 
				cell = 318;

			else if(preset_unit_pointer->table_index == 371) 
				cell = 301; //Countess Chest

			//Act 2 orifice
			else if(preset_unit_pointer->table_index == 152) 
				cell = 300;

			//Frozen Anya
			else if(preset_unit_pointer->table_index == 460) 
				cell = 1468;

			//Canyon/Arcane Waypoint
			else if((preset_unit_pointer->table_index == 402) && (room_type_2_pointer->level->level_number == 46))
				cell = 0;

			else if((preset_unit_pointer->table_index == 267) && (room_type_2_pointer->level->level_number != 75) && (room_type_2_pointer->level->level_number != 103))
				cell = 0;

			else if((preset_unit_pointer->table_index == 376) && (room_type_2_pointer->level->level_number == 107))
				cell = 376;

			if(cell == -1)
			{
				//Check for stupid objects.

				if(preset_unit_pointer->table_index > 574)
					cell = preset_unit_pointer->table_index - 574;
				else
				{
					//Get the object cell
					object_table_entry * object_table_entry_pointer = d2_get_object_table_entry(preset_unit_pointer->table_index);

					//Check if it's a shrine
					if(object_table_entry_pointer->automap == 310)
						//Check to see if it is a bad shrine.
						cell = (object_table_entry_pointer->sub_class & 1) ? 310 : 0;
					else
						//Set the cell number then.
						cell = object_table_entry_pointer->automap;
				}
			}
		}

		if((cell > 0) && (cell < 574))
		{
			automap_cell * automap_cell_pointer = d2_new_automap_cell();

			automap_cell_pointer->cell_number = cell;
			int x = (preset_unit_pointer->position_x + (room_type_2_pointer->position_x * 5));
			int y = (preset_unit_pointer->position_y + (room_type_2_pointer->position_y * 5));
			automap_cell_pointer->x = (((x - y) * 16) / 10) + 1;
			automap_cell_pointer->y = (((y + x) * 8) / 10) - 3;

			d2_add_automap_cell(automap_cell_pointer, get_layer_objects_pointer());
		}
	}
}
