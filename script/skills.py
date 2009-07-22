import configuration, craw

class skill_handler_class:
	def __init__(self):
		self.skills = {
				105: configuration.faster_cast_rate,
				102: configuration.faster_block_rate,
				99: configuration.faster_hit_recovery,
				93: configuration.increased_attack_speed,
				95: configuration.faster_run_walk
		}
		
	def set_skills(self):
		#print 'Setting the skills'
		for skill in self.skills:
			value = self.skills[skill]
			packet = '\x1d' + chr(skill) + chr(value)
			
			craw.receive_packet(packet)
		
	def process_bytes(self, bytes):
		if len(bytes) == 1 and bytes[0] == 0x04:
			self.set_skills()