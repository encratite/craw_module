import utility, craw, configuration, packets, time

class enchant_handler_class:
	enchant_skill = 0x0034
	
	def __init__(self):
		self.mercenary_map = {}
		
	def perform_mana_check(self, cast_count):
		mana_usage = cast_count * (self.enchant_level + 24)
		current_mana, maximum_mana = self.mana
		output = mana_usage <= current_mana
		if not output:
			packets.send_chat(configuration.enchant_mana_error % self.mana)
		else:
			current_mana -= mana_usage
			self.mana = current_mana, maximum_mana
		return output
		
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
			if not self.perform_pre_cast_check(1):
				return
			if self.enchant_player(name):
				packets.send_chat(configuration.enchant_confirmation % self.mana)
				
		elif message == configuration.enchant_mercenary_command:
			if not self.perform_pre_cast_check(1):
				return
			if self.enchant_mercenary(name):
				packets.send_chat(configuration.enchant_mercenary_confirmation % self.mana)
				
		elif message == configuration.enchant_both_command:
			if not self.perform_pre_cast_check(2) or not self.enchant_player(name):
				return
			time.sleep(configuration.enchant_delay)
			if self.enchant_mercenary(name):
				packets.send_chat(configuration.enchant_both_confirmation % self.mana)
		
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
				
	def enchant_player(self, name):
		if not self.distance_check():
			packets.send_chat(configuration.enchant_range_error)
			return False
		
		self.enchant(self.player.id, 0)
		return True
		
	def enchant_mercenary(self, name):
		if not self.distance_check():
			packets.send_chat(configuration.enchant_range_error)
			return False
			
		try:
			mercenary_act, mercenary_id = self.mercenary_map[self.player.id]
		except KeyError:
			packets.send_chat(configuration.enchant_mercenary_error)
			return False
		
		self.enchant(mercenary_id, 1)
		return True
			
	def enchant(self, target, type):
		packets.set_right_skill(enchant_handler_class.enchant_skill)
		packets.cast_right_skill_at_target(type, target)