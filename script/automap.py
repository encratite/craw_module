from automap_configuration import *
from utility import *
from text import *

class automap_handler_class:
	def is_minion(self):
		return get_flag(self.monster_data.monster_flags, 4)
		
	def is_champion(self):
		return get_flag(self.monster_data.monster_flags, 2)
		
	def is_boss(self):
		return get_flag(self.monster_data.monster_flags, 3)
		
	def is_npc(self):
		return get_flag(self.monster_data.flags, 9)
		
	def get_special_abilities_string(self):
		output = ''
		
		for special_ability in self.monster_data.special_abilities:
			try:
				colour, string = special_abilities[special_ability]
				output += colour + string
			except KeyError:
				continue
		
		return output
	
	def get_immunity_string(self):
		immunities = [
			self.monster_data.fire_resistance,
			self.monster_data.cold_resistance,
			self.monster_data.lightning_resistance,
			self.monster_data.poison_resistance,
			self.monster_data.damage_resistance,
			self.monster_data.magic_resistance
		]
		
		output = ''
		offset = 0
		for immunity in immunities:
			if immunity >= 100:
				output += immunity_colours[offset] + immunity_symbol
			
			offset += 1
			
		return output
	
	def process_data(self, monster_data):
		#craw.draw_text(get_rainbow(), 32, 32, False)
		
		self.monster_data = monster_data
		
		coordinate = (monster_data.x, monster_data.y)
		
		if monster_data.type != 1:
			if monster_data.colour != 0x00:
				draw_box(coordinate, monster_data.colour)
			return
			
		if monster_data.colour != 0x00:
			draw_box(coordinate, monster_data.colour)
		else:
			if monster_data.mode in [0, 12]:
				return
				
			if monster_data.treasure_class[0] == 0:
				return
				
			special_abilities = False
				
			if self.is_boss():
				colour = boss_colour
				special_abilities = True
			elif self.is_champion():
				colour = champion_colour
				special_abilities = True
			elif self.is_minion():
				colour = minion_colour
			else:
				colour = monster_colour
				
			immunity_string = self.get_immunity_string()
			if special_abilities:
				special_abilities_string = self.get_special_abilities_string()
			else:
				special_abilities_string = ''
			monster_text = immunity_string + special_abilities_string
				
			if len(monster_text) > 0:
				draw_automap_text(monster_text, coordinate)
				
			draw_box(coordinate, colour)