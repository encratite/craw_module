import craw, utility, configuration, time

class chicken_handler_class:
	def __init__(self):
		self.damage_handlers = []
		self.hostility_handlers = []
		self.last_maximal_life = None
		
	def life_check(self, bytes):
		if len(bytes) < 3:
			return
			
		configuration.chicken_data = craw.get_life()
		if configuration.chicken_data == None:
			return
			
		current_life, maximal_life = configuration.chicken_data
		
		if maximal_life > 0 and maximal_life < self.last_maximal_life and self.last_maximal_life != None:
			craw.print_text('Warning: Maximal life decreased to %d' % maximal_life)
			
		self.last_maximal_life = maximal_life
			
		if bytes[0] not in [0x18, 0x95]:
			return
			
		new_life = utility.read_bytes(bytes, 1, 2) & 0x7fff
		if new_life >= current_life:
			return
			
		damage = current_life - new_life
		ratio = float(new_life) / maximal_life
		percent = '%.2f%%' % (ratio * 100)
		
		if new_life == maximal_life:
			#hack for a strange bug
			return
		
		#print time.time()
		print '%d damage, %d/%d left (%s)' % (damage, new_life, maximal_life, percent)
		for damage_handler in self.damage_handlers:
			damage_handler(damage, (new_life, maximal_life))
		
		if new_life <= 0:
			print 'I am dead.'
			return
		
		in_town = utility.town_check()
		
		if in_town == None:
			return
		
		if not in_town and configuration.chicken and ratio <= configuration.chicken_ratio:
			print 'Leaving the game because the chicken life ratio has been reached'
			craw.leave_game()
			
	def add_damage_handler(self, handler):
		self.damage_handlers.append(handler)
		
	def add_hostility_handler(self, handler):
		self.hostility_handlers.append(handler)
			
	def hostility_check(self, bytes):
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
			
		for hostility_handler in self.hostility_handlers:
			hostility_handler()
			
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
		self.hostility_check(bytes)