import configuration, utility, packets, craw, threading, time

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
		time.sleep(0.4)
		portal_id = utility.pack_number(self.tp_entry.id, 4)
		craw.send_packet('\x04\x02\x00\x00\x00' + portal_id)
		time.sleep(0.05)
		craw.send_packet('\x13\x02\x00\x00\x00' + portal_id)
		#print 'Done entering the TP'

class follow_handler_class:
	def __init__(self):
		self.following = False
		self.town_portal_map = {}
		self.current_portal = None
		
	def command_match(self, input, command):
		return input == command or input == '%s, %s' % (self.my_name, command)
		
	def process_command(self, name, message):
		self.my_name = utility.get_my_name()
		player = utility.get_player_by_name(name)
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
				
		elif self.command_match(message, configuration.enter_tp_command):
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
			if my_name not in configuration.followers or name not in configuration.follow_leaders:
				return
			
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