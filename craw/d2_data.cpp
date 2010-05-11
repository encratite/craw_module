#include "d2_data.hpp"

bool read_flag(unsigned flags, unsigned offset)
{
	return ((flags >> offset) & 1) != 0;
}

bool monster_data::is_minion()
{
	return get_flag(4);
}

bool monster_data::is_champion()
{
	return get_flag(2);
}

bool monster_data::is_boss()
{
	return get_flag(3);
}

bool monster_data::get_flag(unsigned offset)
{
	return read_flag(flags, offset);
}

bool monster_statistics::is_npc()
{
	//use interact flag, haha.
	return read_flag(flags, 9);
}

std::string item_text::get_code() const
{
	return std::string(code, 3);
}

std::string roster_unit::get_name()
{
	std::string output;

	for(std::size_t i = 0; i < sizeof(name); i++)
	{
		char letter = name[i];
		if(letter == 0)
			break;
		output.push_back(letter);
	}

	return output;
}
