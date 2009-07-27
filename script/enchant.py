import utility, craw, configuration, packets, time

class enchant_handler_class:
	enchant_skill = 0x0034
	
	def process_command(self, name, message):
		if message == configuration.enchant_command:
			print 'Enchanting %s' % name
			if self.enchant_player(name):
				packets.send_chat(configuration.enchant_confirmation % self.mana)
		
	def process_bytes(self, bytes):
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
				
	def enchant_player(self, name):
		player = utility.get_player_data_by_name(name)
		my_player = utility.get_my_player()
		
		my_coordinates = (my_player.x, my_player.y)
		player_coordinates = (player.x, player.y)
		
		distance = utility.distance(my_coordinates, player_coordinates)
		
		if player.x == 0 or distance > configuration.maximal_enchant_distance:
			packets.send_chat(configuration.enchant_range_error)
			return False
		
		return self.enchant(player.id)
			
	def enchant(self, target):
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
		packets.cast_right_skill_at_target(0, target)
		return True