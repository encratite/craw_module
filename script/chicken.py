from craw import get_life, leave_game, get_player_level, get_player_id
from utility import read_bytes
from configuration import chicken, chicken_ratio, chicken_on_hostile

class chicken_handler_class:
	def town_check(self):
		level = get_player_level()
		if level == None:
			return None
		return level in [0x01, 0x28, 0x4b, 0x67, 0x6d]
		
	def life_check(self, bytes):
		if len(bytes) < 3:
			return
			
		chicken_data = get_life()
		if chicken_data == None:
			return
			
		current_life, maximum_life = chicken_data
			
		if bytes[0] not in [0x18, 0x95]:
			return
			
		new_life = read_bytes(bytes, 1, 2) & 0x7fff
		if new_life >= current_life:
			return
			
		damage = current_life - new_life
		ratio = float(new_life) / maximum_life
		percent = '%.2f%%' % (ratio * 100)
		
		print '%d damage, %d/%d left (%s)' % (damage, new_life, maximum_life, percent)
		
		in_town = self.town_check()
		
		if in_town == None:
			return
		
		if not in_town and chicken and ratio <= chicken_ratio:
			print 'Leaving the game because the chicken life ratio has been reached'
			leave_game()
			
	def hostile_check(self, bytes):
		if len(bytes) < 10 or bytes[0] != 0x8c:
			return
			
		player_id_1 = read_bytes(bytes, 1, 4)
		player_id_2 = read_bytes(bytes, 5, 4)
		
		my_id = get_player_id()
		if my_id == None:
			return
		
		if player_id_1 != my_id:
			return
			
		if bytes[9] < 0x08:
			return
			
		in_town = self.town_check()
		
		if in_town == None:
			return
			
		if not in_town and chicken_on_hostile:
			print 'Leaving the game because a player has declared hostility against us'
			leave_game()
		
	def process_bytes(self, bytes):
		self.life_check(bytes)
		self.hostile_check(bytes)