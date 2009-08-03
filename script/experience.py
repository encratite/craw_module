from experience_table import *
import utility, math, craw, packets, configuration

class experience_handler_class:
	def process_bytes(self, bytes):
		experience_formats = [
			(0x1a, 1),
			(0x1b, 2)
		]
		
		if packets.entering_game(bytes):
			self.experience = None
			
		elif len(bytes) >= 5 and bytes[0] == 0x1c:
			new_experience = utility.read_bytes(bytes, 1, 4)
			if self.experience == None:
				self.experience = new_experience
			else:
				self.process_experience(new_experience - self.experience)
		
		else:
			for identifier, size in experience_formats:
				if len(bytes) >= 1 + size and bytes[0] == identifier:
					experience_gained = utility.read_bytes(bytes, 1, size)
					self.process_experience(experience_gained)
					break
				
	def process_experience(self, experience_gained):
		player = utility.get_my_player()
		if player == None:
			return
			
		experience_for_this_level = experience_table[player.level - 1]
		experience_for_next_level = experience_table[player.level]
		current_experience = self.experience
		remaining_experience = experience_for_next_level - current_experience
		kills_required = int(math.ceil(float(remaining_experience) / experience_gained))
		experience_difference = current_experience - experience_for_this_level
		experience_in_this_level = experience_for_next_level - experience_for_this_level
		progress = float(experience_difference) / experience_in_this_level * 100.0
		self.experience += experience_gained
		if kills_required > configuration.experience_peanut_limit:
			return
		print 'Gained %d experience points, kill %d more of these (%.2f%%)' % (experience_gained, kills_required, progress)