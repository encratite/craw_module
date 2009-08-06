import craw, packets, utility, time, nil.thread

def get_distance(player1, player2):
	dx = abs(player1.x - player2.x)
	dy = abs(player1.y - player2.y)
	return (dx + dy + max(dx, dy)) / 2.0
	
def sort_players(left, right):
	difference = left[0] - right[0]
	if difference == 0:
		return 0
	return 1 if difference > 0 else -1
	
teleport_skill = 0x36
bone_spear_skill = 0x54
bone_prison_skill = 0x58
bone_spirit_skill = 0x5d

#These are self-describing 'enums'
attack_mode_bone_prison_bone_spirit_tppk = 'Cast Bone Prison at the target, teleport right on it, cast one Bone Spirit at it and TPPK'
attack_mode_bone_prison_bone_spear_tppk = 'Cast Bone Prison and one Bone Spear at the target and TPPK'
attack_mode_cast_left_skill = 'Casts the left skill at the target'
	
class player_killer_class:
	def __init__(self, town_portal_handler):
		self.movement = {}
		self.attack_mode = None
		self.town_portal_handler = town_portal_handler		
		
		self.debugging = True
		
		self.maximal_distance = 100
		
		self.town_portal_delay = 0.25
		self.bone_prison_delay = 0.25
		
		self.left_skill = None
		
	def is_in_range(self, id):
		players = filter(lambda x: x.id == id, craw.get_players())
		if len(players) == 0:
			return False
		player = players[0]
		return player.x != 0
		
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
				self.debug('Right skill has been changed, casting teleport')
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
			if self.attack_mode == attack_mode_bone_prison_bone_spirit_tppk and my_player.id == unit_id:
				self.debug('Detected reassignment, performing attack')
				
				if not self.is_in_range(self.target.id):
					print 'The player is no longer in range - unable to cast Bone Spirit'
					self.attack_mode = None
					return
					
				packets.cast_left_skill_at_target(0, self.target.id)
				nil.thread.create_thread(self.town_portal)
				
	def town_portal(self):
		time.sleep(self.town_portal_delay)
				
		if not self.is_in_range(self.target.id):
			print 'The player is no longer in range - aborting TPPK sequence'
			self.attack_mode = None
			return
		
		self.debug('Casting town portal')
		self.town_portal_handler.tppk(self.target.id)
		self.attack_mode = None
				
	def bone_prison_teleport(self):
		time.sleep(self.bone_prison_delay)
		packets.set_right_skill(teleport_skill)
		
	def bone_prison_bone_spear(self):
		time.sleep(self.bone_prison_delay)
		if not self.is_in_range(self.target.id):
			print 'The player is no longer in range - unable to cast Bone Spear'
			self.attack_mode = None
			return
		packets.cast_left_skill_at_target(0, self.target.id)
		self.town_portal()
				
	def debug(self, message):
		if self.debugging:
			print '%s (%.3f)' % (message, time.time())
			
	def skill_check(self, skills, attack_mode):
		for skill in skills:
			if craw.get_skill_level(skill) == 0:
				print 'You lack a skill to perform the following attack: %s' % attack_mode
				return False
		print 'Performing attack "%s"' % attack_mode
		self.attack_mode = attack_mode
		return True

	def attack(self):
		if utility.town_check():
			print 'You cannot attack other players in town.'
			return
		
		players = craw.get_players()
		my_player = utility.get_my_player()
		players = filter(lambda x: x.level >= 9 and x.x != 0 and x.id != my_player.id, players)
		print 'First filter: %d' % len(players)
		players = map(lambda x: (get_distance(my_player, x), x), players)
		players = filter(lambda x: x[0] <= self.maximal_distance, players)
		print 'Second filter: %d' % len(players)
		players.sort(cmp = sort_players)
		
		if len(players) == 0:
			print 'There are no suitable targets in range.'
			return
		
		print 'List of targets:'
		for distance, player in players:
			print '%s: %.2f (%d, %d)' % (player.name, distance, player.x, player.y)
			
		target = players[0][1]
		
		if target.id in self.movement:
			x, y = self.movement[target.id]
			print 'It is a moving target: %d, %d' % (x, y)
			
		static_distance = 0
			
		teleport_x = target.x - static_distance
		teleport_y = target.y - static_distance
		
		self.target = target
		self.teleport_location = (teleport_x, teleport_y)
		
		bone_spirit_attack = self.left_skill == bone_spirit_skill and self.skill_check([bone_prison_skill, teleport_skill], attack_mode_bone_prison_bone_spirit_tppk)
		bone_spear_attack = self.left_skill == bone_spear_skill and self.skill_check([bone_prison_skill], attack_mode_bone_prison_bone_spear_tppk)
		
		if bone_spirit_attack or bone_spear_attack:
			self.debug('Setting the right skill to Bone Prison')
			packets.set_right_skill(bone_prison_skill)
		else:
			print 'Performing default attack "%s"' % attack_mode_cast_left_skill
			packets.cast_left_skill_at_target(0, self.target.id)