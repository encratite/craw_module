import craw, nil.time, utility, threading

class player_information:
	def __init__(self, name):
		self.name = name
		player_data = utility.get_player_data_by_name(name)
		if player_data == None:
			self.character_class_string = '[Unknown]'
			print 'Unable to extract player information for name %s' % name
		else:
			self.character_class_string = utility.get_character_string(player_data)
		self.account = None
		
	def get_string(self):
		return '%s (*%s) (%s)' % (self.name, self.account, self.character_class_string)
		

class player_kill_whois_request:
	def __init__(self, victim_name, killer_name):
		self.victim = player_information(victim_name)
		self.killer = player_information(killer_name)

class player_kill_handler_class:
	def __init__(self):
		self.bncs_handler = None
		self.request_lock = threading.Lock()
		
		self.pending_requests = []
	
	def victim_whois_handler(self, account):
		self.request_lock.acquire()
		self.pending_requests[0].victim.account = account
		self.request_lock.release()
		
	def killer_whois_handler(self, account):
		self.request_lock.acquire()
		request = self.pending_requests[0]
		self.pending_requests = self.pending_requests[1 : ]
		request.killer.account = account
		print '%s %s was slain by %s' % (nil.time.timestamp(), request.victim.get_string(), request.killer.get_string())
		self.request_lock.release()

	def process_bytes(self, bytes):
		if len(bytes) < 26:
			return
			
		if bytes[0 : 2] != [0x5a, 0x06]:
			return
			
		victim_name = utility.read_name(bytes, 8)
		killer_name = utility.read_name(bytes, 24)
		
		victim = utility.get_player_data_by_name(victim_name)
		killer = utility.get_player_data_by_name(killer_name)
		
		if victim == None or killer == None:
			print 'Somebody got killed but the program was unable to extract the victim/killer'
			return
		
		victim_string = utility.get_character_string(victim)
		killer_string = utility.get_character_string(killer)
		
		line = '%s %s (%s) was slain by %s (%s) ' % (nil.time.timestamp(), victim_name, victim_string, killer_name, killer_string)
		print line
		
		self.request_lock.acquire()
		self.pending_requests.append(player_kill_whois_request(killer_name, victim_name))
		self.request_lock.release()
		
		self.bncs_handler.whois(victim_name, self.victim_whois_handler)
		self.bncs_handler.whois(killer_name, self.killer_whois_handler)