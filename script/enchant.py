import utility, craw, configuration, packets, time, nil.thread, threading

class enchant_handler_class:
	enchant_skill = 0x0034
	
	def __init__(self):
		self.mercenary_map = {}
		self.running = False
		self.lock = threading.Lock()
		
	def perform_mana_check(self, cast_count):
		self.mana_per_cast = self.enchant_level + 24
		mana_usage = cast_count * self.mana_per_cast
		current_mana, maximum_mana = self.mana
		output = mana_usage <= current_mana
		if self.mana_per_cast > current_mana:
			packets.send_chat(configuration.enchant_mana_error % self.mana)
			return False
		elif mana_usage > current_mana:
			packets.send_chat(configuration.enchant_mana_lack_warning % self.mana)
			return True
		else:
			return True
		
	def perform_party_check(self):
		output = self.player.level_id != 0
		if not output:
			packets.send_chat(configuration.party_error)
		return output
		
	def perform_pre_cast_check(self, cast_count):
		if not self.perform_mana_check(cast_count):
			return False
		return self.perform_party_check()
	
	def process_command(self, name, message):
		self.player = utility.get_player_data_by_name(name)
		self.my_player = utility.get_my_player()
		
		if self.player.id == self.my_player.id:
			return
		
		if self.player == None or self.my_player == None:
			return
		
		enchant_level = craw.get_skill_level(enchant_handler_class.enchant_skill)
		if enchant_level == None:
			print 'Unable to retrieve the enchant skill level'
			return
			
		if enchant_level == 0:
			print 'Unable to enchant - this character does not even have this skill!'
			return
		
		self.enchant_level = enchant_level
	
		if message == configuration.enchant_command:
			self.launch_function(self.enchant_player)
				
		elif message == configuration.enchant_minions_command:
			self.launch_function(self.enchant_minions)
				
		elif message == configuration.enchant_all_command:
			self.launch_function(self.enchant_all)
			
	def launch_function(self, function):
		self.lock.acquire()
		if self.running:
			print 'Received an enchant request that was blocked because the thread is still running.'
		else:
			self.running = True
			nil.thread.create_thread(function)
		self.lock.release()
			
	def enchant_player(self):
		if not self.distance_check() or not self.perform_pre_cast_check(1):
			return False
		self.enchant(self.player.id, 0)
		packets.send_chat(configuration.enchant_confirmation % self.mana)
		
	def process_minions(self):
		minions = craw.get_minions(self.player.id)
		if minions == None:
			print 'Unable to retrieve the minions of player %s' % self.player.name
			return None
			
		minions = filter(lambda minion: not minion.enchanted, minions)
		minion_count = len(minions)
		
		if not self.perform_pre_cast_check(minion_count):
			return None
			
		minions_enchanted = 0
		for minion in minions:
			if not self.enchant(minion.id, 1):
				break
			minions_enchanted += 1
			
		return minions_enchanted
	
	def enchant_minions(self):
		if not self.distance_check():
			return False
			
		minions_enchanted =  self.process_minions()
		if minions_enchanted == None:
			return False
		
		packets.send_chat(configuration.enchant_confirmation % (minions_enchanted, self.mana))
		return True
	
	def enchant_all(self):
		if not self.distance_check():
			return False
			
		self.enchant(self.player.id, 0)
		
		minions_enchanted =  self.process_minions()
		if minions_enchanted == None:
			return False
			
		packets.send_chat(configuration.enchant_all_confirmation % (minions_enchanted, self.mana))
		return True
		
	def process_bytes(self, bytes):
		mercenary = packets.assign_mercenary(bytes)
		if mercenary != None:
			mercenary_act, owner_id, mercenary_id = mercenary
			self.mercenary_map[owner_id] = (mercenary_act, mercenary_id)

		my_name = utility.get_my_name()
		if my_name not in configuration.enchanters:
			return
			
		self.mana = craw.get_mana()
		if self.mana == None:
			print 'Unable to retrieve the mana values'
			return
			
		message = packets.parse_message(bytes)
		if message != None:
			name, message = message
			if configuration.remote_command_privilege_users == None or name in configuration.remote_command_privilege_users:
				self.process_command(name, message)
			
		removal = packets.object_removal(bytes)
		if removal != None:
			type, id = removal
			for player_id in self.mercenary_map:
				if self.mercenary_map[player_id][1] == id:
					del self.mercenary_map[player_id]
					break
					
	def distance_check(self):
		my_coordinates = (self.my_player.x, self.my_player.y)
		player_coordinates = (self.player.x, self.player.y)
		
		distance = utility.distance(my_coordinates, player_coordinates)
		
		return self.player.x != 0 and distance <= configuration.maximal_enchant_distance
			
	def enchant(self, target, type):
		packets.set_right_skill(enchant_handler_class.enchant_skill)
		packets.cast_right_skill_at_target(type, target)
		current_mana, maximum_mana = self.mana
		current_mana -= self.mana_per_cast
		self.mana = (current_mana, maximum_mana)
		time.sleep(configuration.enchant_delay)
		return current_mana >= self.mana_per_cast