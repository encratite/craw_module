from craw import get_life, leave_game
from utility import read_bytes
from configuration import chicken, chicken_ratio, chicken_on_hostile

class chicken_handler_class:
	def process_bytes(self, bytes):
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
		if chicken and ratio <= chicken_ratio:
			print 'Leaving the game because the chicken life ratio has been reached'
			leave_game()