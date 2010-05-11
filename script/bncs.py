import nil.string, nil.thread, utility, craw, packets, time, configuration, threading

class bncs_packet_handler_class:
	def __init__(self):
		self.lock = threading.Lock()
		self.whois_name_queue = []
		self.whois_handler_queue = []
		self.account_map = {}
		self.entering_game = False
		self.has_thread = False
		
	def process_bytes(self, bytes):
		if not self.has_thread:
			#print 'Creating whois thread'
			nil.thread.create_thread(self.whois_thread)
			self.has_thread = True
			
		if packets.entering_game(bytes):
			self.entering_game = True
			
		assignment = packets.parse_assignment(bytes)
		if assignment != None and self.entering_game:
			player_id, character_class, player_name, x, y = assignment
			#print 'Assignment: %s' % player_name
			self.entering_game = False
			self.whois(player_name, None)
		
	def process_packet(self, packet):
		if len(packet) >= 2 and packet[0 : 2] == '\xff\x0f':
			account = nil.string.extract_string(packet, '(*', ')')
			if account == None:
				return
				
			handler = None
				
			self.lock.acquire()
			if len(self.whois_handler_queue) > 0:
				name, handler = self.whois_handler_queue[0]
				self.account_map[name] = account
				self.whois_handler_queue = self.whois_handler_queue[1 : ]
			self.lock.release()
			
			if handler != None:
				handler(account)
			
	def whois(self, name, handler):
		if name == None:
			print 'Received /whois None request'
			return
			
		#print 'Running whois on %s: %s' % (name, repr(self.account_map))
		if name in self.account_map:
			#print '%s was cached' % name
			if handler != None:
				handler(self.account_map[name])
			return
			
		self.lock.acquire()
		self.whois_name_queue.append(name)
		self.whois_handler_queue.append((name, handler))
		self.lock.release()
			
	def whois_thread(self):
		while True:
			self.lock.acquire()
			if len(self.whois_name_queue) > 0:
				name = self.whois_name_queue[0]
				#print 'Whois thread is processing %s' % name
				command = '/whois %s' % name
				packet = '\xff\x0e' + utility.pack_number(len(command) + 5, 1) + '\x00' + command + '\x00'
				#Missing BNCS connection check?
				craw.send_bncs_packet(packet)
				self.whois_name_queue = self.whois_name_queue[1 : ]
			self.lock.release()
			time.sleep(configuration.whois_delay)