from experience_table import *
import utility, math, craw, packets

class experience_handler_class:
	def process_bytes(self, bytes):
		experience_formats = [
			(0x1a, 1),
			(0x1b, 2),
			(0x1c, 4)
		]
		
		if bytes[0] == 0x02:
			self.experience = None
		
		else:
			for identifier, size in experience_formats:
				if len(bytes) >= 1 + size and bytes[0] == identifier:
					new_experience = utility.read_bytes(bytes, 1, size)
					if self.experience != None:
						self.process_experience(new_experience - self.experience)
					self.experience = new_experience
					break
				
	def process_experience(self, experience):
		player = utility.get_my_player()
		if player == None:
			return
			
		experience_for_this_level = experience_table[player.level - 1]
		experience_for_next_level = experience_table[player.level]
		current_experience = self.experience
		remaining_experience = experience_for_next_level - current_experience
		kills_required = int(math.ceil(float(remaining_experience) / experience))
		experience_difference = current_experience - experience_for_this_level
		experience_in_this_level = experience_for_next_level - experience_for_this_level
		progress = float(experience_difference) / experience_in_this_level * 100.0
		print 'Gained %d experience points, kill %d more of these (%.2f%%)' % (experience, kills_required, progress)