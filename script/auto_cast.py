import packets, configuration, nil.thread, craw

auto_cast_skills = [
	('Bone Armor', 0x44),
	('Clay Golem', 0x4b),
	('Holy Shield', 0x75),
	('Oak Sage', 0xe2),
	('Cyclone Armor', 0xeb),
]

class auto_cast_handler_class:
	def __init__(self):
		self.entering = False
		
	def process_bytes(self, bytes):
		if packets.entering_game(bytes):
			self.entering = True
			
		if self.entering and packets.load_complete(bytes):
			self.auto_cast()
			self.entering = False
			
	def auto_cast(self):
		nil.thread.create_thread(self.auto_cast_thread)
		
	def auto_cast_thread(self):
		for name, skill in auto_cast_skills:
			level = craw.get_skill_level(skill)
			if level == None or level == 0:
				continue
				
			print 'Casting %s level %d' % (name, level)
			packets.perform_cast(skill, configuration.auto_cast_switch_delay, configuration.auto_cast_cast_delay)