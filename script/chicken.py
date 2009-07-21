import craw, utility, configuration

class chicken_handler_class:
	def life_check(self, bytes):
		if len(bytes) < 3:
			return
			
		configuration.chicken_data = craw.get_life()
		if configuration.chicken_data == None:
			return
			
		current_life, maximum_life = configuration.chicken_data
			
		if bytes[0] not in [0x18, 0x95]:
			return
			
		new_life = utility.read_bytes(bytes, 1, 2) & 0x7fff
		if new_life >= current_life:
			return
			
		damage = current_life - new_life
		ratio = float(new_life) / maximum_life
		percent = '%.2f%%' % (ratio * 100)
		
		print '%d damage, %d/%d left (%s)' % (damage, new_life, maximum_life, percent)
		
		in_town = utility.town_check()
		
		if in_town == None:
			return
		
		if not in_town and configuration.chicken and ratio <= configuration.chicken_ratio:
			print 'Leaving the game because the configuration.chicken life ratio has been reached'
			craw.leave_game()
			
	def hostile_check(self, bytes):
		if len(bytes) < 10 or bytes[0] != 0x8c:
			return
			
		player_id_1 = utility.read_bytes(bytes, 1, 4)
		player_id_2 = utility.read_bytes(bytes, 5, 4)
		
		my_id = craw.get_player_id()
		if my_id == None:
			return
		
		if player_id_1 != my_id:
			return
			
		if bytes[9] < 0x08:
			return
			
		in_town = utility.town_check()
		
		if in_town == None:
			return
			
		if not in_town and configuration.chicken_on_hostile:
			player_name = craw.get_name_by_id(player_id_2)
			if player_name == None:
				print 'Leaving the game because an unknown player has declared hostility against us'
			else:
				print 'Leaving the game because player %s has declared hostility against us' % player_name
			leave_game()
		
	def process_bytes(self, bytes):
		self.life_check(bytes)
		self.hostile_check(bytes)