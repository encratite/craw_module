import configuration, utility, packets, craw, threading, time, nil.thread, random, npc

class town_portal_entry:
	def __init__(self, object_id, x, y):
		self.id = object_id
		self.location = (x, y)
		
class town_portal_timer(threading.Thread):
	def __init__(self, tp_entry):
		threading.Thread.__init__(self)
		self.tp_entry = tp_entry
		self.start()
		
	def run(self):
		#print 'Entering TP %08x' % self.tp_entry.id
		x, y = self.tp_entry.location
		craw.send_packet('\x03' + utility.pack_number(x, 2) + utility.pack_number(y, 2))
		time.sleep(configuration.follow_portal_move_delay)
		portal_id = utility.pack_number(self.tp_entry.id, 4)
		craw.send_packet('\x04\x02\x00\x00\x00' + portal_id)
		time.sleep(configuration.follow_portal_interact_delay)
		craw.send_packet('\x13\x02\x00\x00\x00' + portal_id)
		#print 'Done entering the TP'

class follow_handler_class:
	def __init__(self, town_portal_handler):
		self.following = False
		self.town_portal_map = {}
		self.town_portal_handler = town_portal_handler
		self.current_portal = None
		self.attacking = False
		self.monsters = {}
		self.assignments = {}
		
	def command_match(self, input, command):
		return input == command or input == '%s, %s' % (self.my_name, command)
		
	def process_command(self, name, message):
		player = utility.get_player_by_name(name)
		
		if self.my_name == name:
			return
		
		if player == None:
			print 'Unable to retrieve player data of player %s' % player
			return
			
		if self.command_match(message, configuration.follow_command):
			self.following = True
			self.leader = player.name
			self.leader_id = player.id
			print 'Following %s' % self.leader
			packets.send_chat(configuration.follow_confirmation % self.leader)
			
		elif self.command_match(message, configuration.stop_command):
			if self.following:
				self.following = False
				print 'No longer following %s' % self.leader
				packets.send_chat(configuration.stop_confirmation % self.leader)
			else:
				print '%s asked us to stop following the leader but we are currently not following anybody' % name
				packets.send_chat(configuration.stop_error)
				
		elif self.command_match(message, configuration.enter_tp_command) and not utility.town_check():
			self.enter_tp(player)
			
		elif self.command_match(message, configuration.enter_town_tp_command) and utility.town_check():
			self.enter_tp(player)
				
		elif self.command_match(message, configuration.go_to_town_command):
			self.town_portal_handler.town_tp()
			
		elif self.command_match(message, configuration.attack_command):
			if self.attacking:
				packets.send_chat(configuration.attack_error)
			else:
				packets.send_chat(configuration.attack_confirmation)
				self.attacking = True
				nil.thread.create_thread(self.attack_thread)
				
		elif self.command_match(message, configuration.stop_attacking_command):
			if self.attacking:
				packets.send_chat(configuration.stop_attacking_confirmation)
				self.attacking = False
			else:
				packets.send_chat(configuration.stop_attacking_error)
			
	def enter_tp(self, player):
		try:
			tp_entry = self.town_portal_map[player.id]
		except KeyError:
			packets.send_chat(configuration.tp_error % player.name)
			return
			
		packets.send_chat(configuration.enter_tp_confirmation % player.name)
		town_portal_timer(tp_entry)
		
	def process_bytes(self, bytes):
		message = packets.parse_message(bytes)
		if message != None:
			name, message = message
			
			my_name = utility.get_my_name()
			self.my_name = my_name
			
			if name not in configuration.follow_leaders:
				return
				
			if name != my_name and self.command_match(message, configuration.leave_command):
				print '%s ordered us to leave the game' % name
				craw.leave_game()
			
			if my_name != None and my_name in configuration.followers:
				self.process_command(name, message)
		
		move = packets.parse_move(bytes)
		if move != None:
			player_id, x, y = move
			if self.following and player_id == self.leader_id:
				#print 'Following %s to (%d, %d)' % (self.leader, x, y)
				craw.move_click(x, y)
				
		assignment = packets.town_portal_assignment(bytes)
		if assignment != None:
			object_id, x, y = assignment
			self.current_portal = town_portal_entry(object_id, x, y)
			#print 'Portal assignment: %08x (%d, %d)' % (object_id, x, y)
			
		portal_ownership = packets.portal_ownership(bytes)
		if portal_ownership != None:
			player_id, portal_id = portal_ownership
			if self.current_portal == None:
				print 'Received portal ownership information without a previous object assignment'
			else:
				self.town_portal_map[player_id] = self.current_portal
				x, y = self.current_portal.location
				#print 'Portal ownership detected: Portal %08x (%d, %d) belongs to player %08x' % (portal_id, x, y, player_id)
				
		object_removal = packets.object_removal(bytes)
		if object_removal != None:
			type, id = object_removal
			for player_id in self.town_portal_map:
				if self.town_portal_map[player_id].id == id:
					del self.town_portal_map[player_id]
					#print 'Removed portal %08x by player %08x' % (id, player_id)
					break
					
			try:
				if type == 1:
					del self.monsters[id]
					#print 'Removed unit %08x' % id
			except KeyError:
				pass
					
		add_unit = packets.parse_add_unit(bytes)
		if add_unit != None:
			unit_type, unit_id = add_unit
			if unit_type == 1:
				try:
					location = self.assignments[unit_id]
					self.monsters[unit_id] = location
					#print 'Added unit %08x: %s' % (unit_id, repr(location))
				except KeyError:
					#print 'Unit was added without prior assignment: %08x' % unit_id
					pass
				
		npc_move = packets.parse_npc_move(bytes)
		if npc_move != None:
			unit_id, running, x, y = npc_move
			if unit_id in self.monsters:
				target = (x, y)
				if unit_id in self.monsters:
					self.monsters[unit_id] = target
					#print 'Monster %08x is moving to %s' % (unit_id, repr(target))
				
		npc_assignment = packets.parse_npc_assignment(bytes)
		if npc_assignment != None:
			unit_id, unit_code, x, y, life = npc_assignment
			if unit_code in npc.npcs:
				#print 'Detected an NPC: %08x' % unit_id
				pass
			else:
				location = (x, y)
				self.assignments[unit_id] = location
				#print 'Assignment: %08x -> %s' % (unit_id, repr(location))
			
	def get_targets(self):
		monsters = []
		
		for unit_id in self.monsters:
			monster = craw.get_unit(unit_id, 1)
			#missing town NPC check?
			if monster == None:
				print 'Failed to receive monster data for ID %08x' % unit_id
				continue
				
			#location hack
			x, y = self.monsters[unit_id]
			monster.x = x
			monster.y = y
				
			if monster.mode in [0, 12]:
				#monster is dead
				self.attack_print('Dead monster: %08x' % unit_id)
				continue
				
			distance = utility.get_d2_distance(self.i, monster)
			if distance > configuration.follower_attack_radius:
				self.attack_print('Distance to %08x too great: %.1f (%s)' % (monster.id, distance, repr((monster.x, monster.y))))
				continue
			monsters.append((distance, unit_id))
			
		monsters.sort(cmp = utility.sort_units)
		
		return monsters
		
	def attack_print(self, text):
		#craw.print_text(text)
		pass
					
	def attack_thread(self):
		self.attack_print('Attack thread has been launched')
		while self.attacking:
			self.i = utility.get_my_player()
			if self.i == None:
				return
				
			my_unit = utility.get_my_unit()
			if my_unit == None:
				self.attack_print('Unable to retrieve unit')
				return
				
			standing_still = my_unit.mode == 1
			if standing_still and not utility.town_check():
				targets = self.get_targets()
				if len(targets) == 0:
					self.attack_print('No targets in range')
					pass
				else:
					self.attack_print('Targets in range:')
					for distance, id in targets:
						self.attack_print('%08x: %.1f' % (id, distance))
						
					if targets[0][0] <= configuration.follower_defence_radius:
						#a monster got too close, kill it, quickly!
						target = targets[0][1]
						self.attack_print('Dangerous close proximity monster detected')
					else:
						index = random.randint(0, min(len(targets), configuration.follower_attack_maximal_randomisation_position) - 1)
						target = targets[index][1]
						
					self.attack_print('Attacking target %08x' % target)
					packets.cast_left_skill_at_target(1, target)
			else:
				self.attack_print('Not standing still (player mode is %d)' % my_unit.mode)
				pass
				
			time.sleep(configuration.follower_attack_delay)