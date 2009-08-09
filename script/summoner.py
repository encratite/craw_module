import packets, nil.thread, time, random, utility, configuration, craw

raise_skeleton_skill = 0x46
raise_skeletal_mage_skill = 0x50
revive_skill = 0x5f

skills = [
	('Raising a skeleton', raise_skeleton_skill),
	('Raising a skeletal mage', raise_skeletal_mage_skill),
	('Reviving a monster', revive_skill)
]

class summoner_handler_class:
	def __init__(self):
		pass
		
	def initialise(self):
		self.skeletons = 0
		self.skeleton_mages = 0
		self.revives = 0
		self.monsters = []
		nil.thread.create_thread(self.summoner_thread)
		
	def process_bytes(self, bytes):
		if packets.entering_game(bytes):
			self.initialise()
			
		add_unit = packets.parse_add_unit(bytes)
		if add_unit != None:
			unit_type, unit_id = add_unit
			if unit_type == 1:
				self.monsters.append(unit_id)
			
		object_removal = packets.object_removal(bytes)
		if object_removal != None:
			type, id = object_removal
			try:
				self.monsters.remove(id)
			except ValueError:
				pass
				
	def get_targets(self):
		targets = []
		for unit_id in self.monsters:
			monster = craw.get_unit(unit_id, 1)
			if monster == None:
				print 'Failed to retrieve the unit of monster %08x' % unit_id
				continue
			if monster.mode in [0, 12]:
				targets.append(unit_id)
				
		return targets
			
	def summoner_thread(self):
		while True:
			player = utility.get_my_player()
			if player == None:
				#print 'No summoner unit'
				time.sleep(configuration.summoner_check_delay)
				continue
			break
				
		if player.name not in configuration.summoners:
			#print 'This is not a summoner'
			return
			
		while True:
			my_unit = utility.get_my_unit()
			if my_unit == None:
				print 'Unable to retrieve summoner unit'
				return
				
			standing_still = my_unit.mode == 1
			if standing_still and not utility.town_check():
				targets = self.get_targets()
				if len(targets) == 0:
					craw.print_text('There are no nearby corpses')
				else:
					target = random.choice(targets)
					description, skill = random.choice(skills)
					craw.print_text('%s: %08x' % (description, target))
					packets.set_right_skill(skill)
					time.sleep(configuration.summoner_switch_delay)
					packets.cast_right_skill_at_target(1, skill)
					time.sleep(configuration.summoner_cast_delay)
					continue
				
			time.sleep(configuration.summoner_check_delay)