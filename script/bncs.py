import nil.string, utility, craw, packets, time, configuration, threading

class bncs_packet_handler_class:
	def __init__(self):
		self.lock = threading.Lock()
		self.whois_queue = []
		self.account_map = {}
		self.entering_game = False
		self.last_name = None
		self.last_packet = None
		
	def process_bytes(self, bytes):
		if packets.entering_game(bytes):
			self.entering_game = True
			
		assignment = packets.parse_assignment(bytes)
		if assignment != None and self.entering_game:
			player_id, character_class, player_name, x, y = assignment
			print 'Assignment: %s' % player_name
			self.entering_game = False
			self.whois(player_name, None)
			
		"""
		my_name = utility.get_my_name()
		if self.last_name != my_name:
			print 'Detected a change!'
			print 'Name: %s' % my_name
			print repr(self.last_packet)
			print repr(bytes)
		self.last_name = my_name
		self.last_packet = bytes
		"""
		
	def process_packet(self, packet):
		if len(packet) >= 2 and packet[0 : 2] == '\xff\x0f':
			account = nil.string.extract_string(packet, '(*', ')')
			if account == None:
				return
			if len(self.whois_queue) > 0:
				name, handler = self.whois_queue[0]
				if handler != None:
					handler(account)
				self.account_map[name] = account
				self.whois_queue = self.whois_queue[1 : ]
			
	def whois(self, name, handler):
		if name == None:
			print 'Received /whois None request'
			return
			
		print 'Running whois on %s: %s' % (name, repr(self.account_map))
		if name in self.account_map:
			print '%s was cached' % name
			if handler != None:
				handler(self.account_map[name])
			return
			
		self.whois_queue.append((name, handler))
		command = '/whois %s' % name;
		packet = '\xff\x0e' + utility.pack_number(len(command) + 5, 1) + '\x00' + command + '\x00'
		#print utility.get_packet_string(packet)
		craw.send_bncs_packet(packet)
		if utility.town_check():
			time.sleep(configuration.whois_delay)