import craw, nil.time, utility

class player_kill_handler_class:

	def process_bytes(self, bytes):
		if len(bytes) < 26:
			return
			
		if bytes[0 : 2] != [0x5a, 0x06]:
			return
			
		print utility.get_packet_string(bytes)
			
		victim_name = utility.read_name(bytes, 8)
		killer_name = utility.read_name(bytes, 24)
		
		print 'Victim name: %s' % victim_name
		print 'Killer name: %s' % killer_name
		
		victim = utility.get_player_data_by_name(victim_name)
		killer = utility.get_player_data_by_name(killer_name)
		
		if victim == None or killer == None:
			print 'Somebody got killed but the program was unable to extract the victim/killer'
			return
		
		victim_string = utility.get_character_string(victim)
		killer_string = utility.get_character_string(killer)
		
		line = '%s %s (%s) was slain by %s (%s) ' % (nil.time.timestamp(), victim_name, victim_string, killer_name, killer_string)
		print line