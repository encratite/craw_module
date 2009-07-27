import utility, craw, configuration, packets, time

class enchant_handler_class:
	enchant_skill = 0x0034
	
	def __init__(self):
		self.mercenary_map = {}
	
	def process_command(self, name, message):
		if message == configuration.enchant_command:
			print 'Enchanting %s' % name
			if self.enchant_player(name):
				packets.send_chat(configuration.enchant_confirmation % self.mana)
				
		elif message == configuration.enchant_mercenary_command:
			print 'Enchanting the mercenary of %s' % name
			if self.enchant_mercenary(name):
				packets.send_chat(configuration.enchant_mercenary_confirmation % self.mana)
				
		elif message == configuration.enchant_both_command:
			print 'Enchanting %s and their mercenary' % name
			if not self.enchant_player(name):
				return
			time.sleep(0.2)
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
					
	def distance_check(self, my_player, player):
		my_coordinates = (my_player.x, my_player.y)
		player_coordinates = (player.x, player.y)
		
		distance = utility.distance(my_coordinates, player_coordinates)
		
		return player.x != 0 and distance <= configuration.maximal_enchant_distance
				
	def enchant_player(self, name):
		player = utility.get_player_data_by_name(name)
		my_player = utility.get_my_player()
		
		if not self.distance_check(my_player, player):
			packets.send_chat(configuration.enchant_range_error)
			return False
		
		return self.enchant(player.id, 0)
		
	def enchant_mercenary(self, name):
		player = utility.get_player_data_by_name(name)
		my_player = utility.get_my_player()
		
		if not self.distance_check(my_player, player):
			packets.send_chat(configuration.enchant_range_error)
			return False
			
		try:
			mercenary_act, mercenary_id = self.mercenary_map[player.id]
		except KeyError:
			packets.send_chat(configuration.enchant_mercenary_error)
			return False
		
		return self.enchant(mercenary_id, 1)
			
	def enchant(self, target, type):
		level = craw.get_skill_level(enchant_handler_class.enchant_skill)
		if level == None:
			print 'Unable to retrieve the enchant skill level'
			return False
			
		if level == 0:
			print 'Unable to enchant - this character does not even have this skill!'
			return False
			
		self.enchant_level = level
			
		current_mana, maximum_mana = self.mana
		
		mana_usage = level + 24
		if mana_usage > current_mana:
			packets.send_chat(configuration.enchant_mana_error % self.mana)
			return False
			
		current_mana -= mana_usage
		self.mana = (current_mana, maximum_mana)
		packets.set_right_skill(enchant_handler_class.enchant_skill)
		packets.cast_right_skill_at_target(type, target)
		return True