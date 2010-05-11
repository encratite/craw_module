import craw, packets, utility, time, nil.thread, configuration
	
teleport_skill = 0x36
bone_spear_skill = 0x54
bone_prison_skill = 0x58
bone_spirit_skill = 0x5d
fist_of_heavens_skill = 0x79

#These are self-describing 'enums'
attack_mode_bone_prison_bone_spirit_tppk = 'Cast Bone Prison at the target, teleport right on it, cast one Bone Spirit at it and performs a TPPK'
attack_mode_bone_prison_bone_spear_tppk = 'Cast Bone Prison and one Bone Spear at the target and performs a TPPK'
attack_mode_cast_left_skill = 'Casts the left skill at the target'
attack_mode_cast_left_skill_tppk = 'Casts the left skill at the target and performs a TPPK'
attack_mode_fist_of_heavens_tppk = 'Attempts to teleport to the target first, casts Fist of Heavens at the target and performs a TPPK'

#Research mode chat triggers
chat_trigger_hostile = 'hostile'
chat_trigger_damage = 'damage'
	
class player_killer_class:
	def __init__(self, town_portal_handler):
		self.movement = {}
		self.attack_mode = None
		self.last_timestamp = None
		self.town_portal_handler = town_portal_handler		
		self.debugging = True
		self.left_skill = None
		
		self.research_mode = False
		self.start_of_attack = None
		
	def sleep(self, delay):
		if self.debugging:
			print 'Sleeping: %d ms' % (delay * 1000)
			time.sleep(delay)
		
	def is_in_range(self, id):
		players = filter(lambda x: x.id == id, craw.get_players())
		if len(players) == 0:
			return False
		player = players[0]
		return player.x != 0
		
	def reset_attack(self):
		self.attack_mode = None
		self.last_timestamp = None
		
	def process_bytes(self, bytes):
		set_skill = packets.parse_set_skill(bytes)
		if set_skill != None:
			unit_type, unit_id, side, skill = set_skill
			#if unit_id == my_player.id:
			if side == 0x01:
				self.left_skill = skill
				#print 'Left skill is set to %02x' % skill
	
		my_player = utility.get_my_player()
		if my_player == None:
			return
		
		move = packets.parse_move(bytes)
		if move != None:
			player_id, x, y = move
			self.movement[player_id] = (x, y)
			
		stop = packets.parse_player_stop(bytes)
		if stop != None:
			unit_type, unit_id, x, y, life = stop
			try:
				del self.movement[unit_id]
			except KeyError:
				pass
				
		if set_skill != None:
			if self.attack_mode == attack_mode_bone_prison_bone_spirit_tppk and skill == teleport_skill:
				self.debug('Right skill has been changed, casting teleport for Bone Spirit attack')
				teleport_x = self.target.x + configuration.bone_spirit_teleport_offset[0]
				teleport_y = self.target.y + configuration.bone_spirit_teleport_offset[1]
				packets.cast_right_skill_at_location(teleport_x, teleport_y)
			elif self.attack_mode == attack_mode_fist_of_heavens_tppk and skill == teleport_skill:
				self.debug('Right skill has been changed, casting teleport for Fist of Heavens attack')
				packets.cast_right_skill_at_target(0, self.target.id)
			elif self.attack_mode == attack_mode_bone_prison_bone_spirit_tppk and skill == bone_prison_skill:
				self.debug('Right skill has been changed, casting Bone Prison for the Bone Spirit attack')
				packets.cast_right_skill_at_target(0, self.target.id)
				nil.thread.create_thread(self.bone_prison_teleport)
			elif self.attack_mode == attack_mode_bone_prison_bone_spear_tppk and skill == bone_prison_skill:
				self.debug('Right skill has been changed, casting Bone Prison for the Bone Spear attack')
				packets.cast_right_skill_at_target(0, self.target.id)
				nil.thread.create_thread(self.bone_prison_bone_spear)
				
		reassignment = packets.parse_reassignment(bytes)
		if reassignment != None:
			unit_type, unit_id, x, y, boolean = reassignment
			if self.attack_mode in [attack_mode_bone_prison_bone_spirit_tppk, attack_mode_fist_of_heavens_tppk] and my_player.id == unit_id:
				self.debug('Detected reassignment, performing attack')
				
				if not self.is_in_range(self.target.id):
					print 'The player is no longer in range - unable to cast Bone Spirit'
					self.reset_attack()
					return
					
				packets.cast_left_skill_at_target(0, self.target.id)
				if self.attack_mode == attack_mode_fist_of_heavens_tppk:
					nil.thread.create_thread(lambda: self.town_portal(configuration.fist_of_heavens_delay))
				else:
					nil.thread.create_thread(self.town_portal)
				
		message = packets.parse_message(bytes)
		if self.start_of_attack != None and message != None:
			name, message = message
			if name != utility.get_my_name():
				self.process_message(name, message)
				
	def process_message(self, name, message):
		tokens = message.split(' ')
		if len(tokens) < 2:
			return
		trigger = tokens[0]
		try:
			value = float(tokens[1])
		except ValueError:
			return
			
		difference = self.get_ms(value, self.start_of_attack)
			
		if trigger == chat_trigger_hostile:
			print 'Delay between the start of the attack and the declaration of hostility: %d ms' % difference
			self.hostile_time = value
		elif trigger == chat_trigger_damage:
			euclidean_distance = utility.get_euclidean_distance(self.i, self.target)
			d2_distance = utility.get_d2_distance(self.i, self.target)
			print 'Delay between the declaration of hostility and the damage: %d ms' % self.get_ms(value, self.hostile_time)
			print 'Delay between the start of the attack and the damage: %d ms' % difference
			print 'Initial euclidean distance between the attacker and the victim: %.1f' % euclidean_distance
			print 'Initial Diablo II distance between the attacker and the victim: %.1f' % d2_distance
			if len(tokens) >= 3:
				try:
					damage = int(tokens[2])
					print 'Damage taken by the victim: %d' % damage
				except	ValueError:
					pass
				
	def process_damage(self, damage, life):
		if self.research_mode:
			packets.send_chat('%s %.3f %d' % (chat_trigger_damage, time.time(), damage))
			
	def process_hostility(self):
		if self.research_mode:
			packets.send_chat('%s %.3f' % (chat_trigger_hostile, time.time()))
			
	def milliseconds(self, value):
		return value * 1000.0
				
	def get_ms(self, now, then):
		return self.milliseconds(now - then)
				
	def town_portal(self, delay = configuration.town_portal_delay):
		self.sleep(delay)
				
		if not self.is_in_range(self.target.id):
			print 'The player is no longer in range - aborting TPPK sequence'
			self.reset_attack()
			return
		
		self.debug('Casting town portal')
		self.town_portal_handler.tppk(self.target.id)
		self.reset_attack()
				
	def bone_prison_teleport(self):
		self.sleep(configuration.bone_prison_delay)
		packets.set_right_skill(teleport_skill)
		
	def bone_prison_bone_spear(self):
		self.sleep(configuration.bone_prison_delay)
		if not self.is_in_range(self.target.id):
			print 'The player is no longer in range - unable to cast Bone Spear'
			self.reset_attack()
			return
		packets.cast_left_skill_at_target(0, self.target.id)
		self.town_portal()
				
	def debug(self, message):
		if self.debugging:
			current_time = time.time()
			if self.last_timestamp == None:
				print message
			else:
				print '%s (%d ms)' % (message, self.get_ms(current_time, self.last_timestamp))
			self.last_timestamp = current_time
			
	def skill_check(self, skills, attack_mode):
		for skill in skills:
			if craw.get_skill_level(skill) == 0:
				print 'You lack a skill to perform the following attack: %s' % attack_mode
				return False
		print 'Performing attack "%s"' % attack_mode
		self.attack_mode = attack_mode
		return True
		
	def is_a_friend(self, player):
		if self.research_mode:
			return False
		return player.name in configuration.friends
		
	def get_target(self):
		players = craw.get_players()
		my_player = utility.get_my_player()
		players = filter(lambda x: x.level >= 9 and x.x != 0 and x.id != my_player.id and not self.is_a_friend(x), players)
		players = map(lambda x: (utility.get_d2_distance(my_player, x), x), players)
		players = filter(lambda x: x[0] <= configuration.maximal_attack_distance, players)
		players.sort(cmp = utility.sort_units)
		
		if len(players) == 0:
			print 'There are no suitable targets in range.'
			return None
		
		print 'List of targets:'
		counter = 1
		for distance, player in players:
			print '%d. %s: %.2f (%d, %d)' % (counter, player.name, distance, player.x, player.y)
			counter += 1
			
		target = players[0][1]
		return target

	def attack(self):
		if utility.town_check():
			print 'You cannot attack other players in town.'
			return
			
		self.i = utility.get_my_player()
		
		target = self.get_target()
		if target == None:
			return
		self.target = target
		
		print 'Picked target %s' % target.name
		
		if target.id in self.movement:
			x, y = self.movement[target.id]
			print 'It is a moving target: %d, %d' % (x, y)
		
		bone_spirit_attack = self.left_skill == bone_spirit_skill and self.skill_check([bone_prison_skill, teleport_skill], attack_mode_bone_prison_bone_spirit_tppk)
		bone_spear_attack = self.left_skill == bone_spear_skill and self.skill_check([bone_prison_skill], attack_mode_bone_prison_bone_spear_tppk)
		foh_attack = self.left_skill == fist_of_heavens_skill and self.skill_check([fist_of_heavens_skill], attack_mode_fist_of_heavens_tppk)
		
		self.start_of_attack = time.time()
		
		if bone_spirit_attack or bone_spear_attack:
			self.debug('Setting the right skill to Bone Prison')
			packets.set_right_skill(bone_prison_skill)
		elif foh_attack:
			if craw.get_skill_level(teleport_skill) > 0:
				self.debug('Setting the right skill to teleport')
				packets.set_right_skill(teleport_skill)
			else:
				if utility.get_d2_distance(utility.get_my_player(), target) > configuration.fist_of_heavens_distance:
					print 'You are not in range for a Fist of Heavens attack'
					return
				self.debug('Casting Fist of Heavens at the target')
				packets.cast_left_skill_at_target(0, self.target.id)
				nil.thread.create_thread(lambda: self.town_portal(configuration.fist_of_heavens_delay))
		else:
			print 'Performing default attack "%s"' % attack_mode_cast_left_skill
			packets.cast_left_skill_at_target(0, self.target.id)
			
	def ground_attack(self):
		target = self.get_target()
		print 'Casting left skill at %s' % target.name
		packets.cast_left_skill_at_location(self.target.x, self.target.y)