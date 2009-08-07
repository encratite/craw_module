import utility, craw, configuration, packets, time, nil.thread, threading, privileges

class enchant_handler_class:
	enchant_skill = 0x0034
	
	def __init__(self):
		self.running = False
		self.lock = threading.Lock()
		
	def perform_mana_check(self, cast_count):
		self.mana_per_cast = self.enchant_level + 24
		mana_usage = cast_count * self.mana_per_cast
		current_mana, maximum_mana = self.mana
		output = mana_usage <= current_mana
		if self.mana_per_cast > current_mana:
			packets.send_chat(configuration.mana_error % self.mana)
			return False
		elif mana_usage > current_mana:
			packets.send_chat(configuration.enchant_mana_lack_warning % self.mana)
			return True
		else:
			return True
		
	def perform_party_check(self):
		is_in_party = utility.same_party(self.player, self.my_player)
		if not is_in_party:
			packets.send_chat(configuration.party_error)
		return is_in_party
		
	def distance_check(self):
		my_coordinates = (self.my_player.x, self.my_player.y)
		player_coordinates = (self.player.x, self.player.y)
		
		distance = utility.distance(my_coordinates, player_coordinates)
		
		is_in_range = self.player.x != 0 and distance <= configuration.maximal_enchant_distance
		if not is_in_range:
			packets.send_chat(configuration.enchant_range_error)
		return is_in_range
		
	def perform_pre_cast_check(self):
		if not self.perform_party_check():
			return False
		return self.distance_check()
	
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
		
		self.mana = craw.get_mana()
		if self.mana == None:
			print 'Unable to retrieve mana'
			return
	
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
			nil.thread.create_thread(lambda: self.function_wrapper(function))
		self.lock.release()
		
	def thread_termination(self):
		self.lock.acquire()
		self.running = False
		self.lock.release()
		
	def function_wrapper(self, function):
		function()
		self.thread_termination()
			
	def enchant_player(self):
		if not self.perform_pre_cast_check() or not self.perform_mana_check(1):
			return False
		self.enchant(self.player.id, 0)
		packets.send_chat(configuration.enchant_confirmation % self.mana)
		return True
		
	def get_minions(self):
		minions = craw.get_minions(self.player.id)
		if minions == None:
			print 'Unable to retrieve the minions of player %s' % self.player.name
			return None
			
		minions = filter(lambda minion: not minion.enchanted, minions)
		return map(lambda minion: (1, minion.id), minions)
		
	def process_targets(self, targets):
		if not self.perform_mana_check(len(targets)):
			return None
			
		targets_enchanted = 0
		for type, id in targets:
			if not self.enchant(id, type):
				break
			targets_enchanted += 1 
			
		return targets_enchanted
	
	def enchant_minions(self):
		if not self.perform_pre_cast_check():
			return False
			
		minions = self.get_minions()
		minions_enchanted = self.process_targets(minions)
		if minions_enchanted == None:
			return False
			
		current_mana, maximum_mana = self.mana
		packets.send_chat(configuration.enchant_minions_confirmation % (minions_enchanted, current_mana, maximum_mana))
		return True
	
	def enchant_all(self):
		if not self.perform_pre_cast_check():
			return False
			
		targets = self.get_minions()
		if targets == None:
			print 'Unable to retrieve minions!'
			targets = []
		targets = [(0, self.player.id)] + targets
		targets_enchanted = self.process_targets(targets)
		if targets_enchanted == None:
			return False
			
		minions_enchanted = targets_enchanted - 1
			
		current_mana, maximum_mana = self.mana
		packets.send_chat(configuration.enchant_all_confirmation % (minions_enchanted, current_mana, maximum_mana))
		return True
		
	def process_bytes(self, bytes):
		self.lock.acquire()
		running = self.running
		self.lock.release()
		
		if running:
			return
		
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
			if privileges.has_remote_privileges(name):
				self.process_command(name, message)
			
	def enchant(self, target, type):
		packets.set_right_skill(enchant_handler_class.enchant_skill)
		packets.cast_right_skill_at_target(type, target)
		current_mana, maximum_mana = self.mana
		current_mana -= self.mana_per_cast
		self.mana = (current_mana, maximum_mana)
		time.sleep(configuration.enchant_delay)
		return current_mana >= self.mana_per_cast