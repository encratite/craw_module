import craw, nil.time

class player_kill_handler_class:

	def process_bytes(self, bytes):
		if len(bytes) < 26:
			return
			
		if bytes[0 : 2] != [0x5a, 0x06]:
			return
			
		victim_name = read_name(bytes, 8)
		killer_name = read_name(bytes, 24)
		
		victim = get_player_data_by_name(victim_name)
		killer = get_player_data_by_name(killer_name)
		
		victim_string = get_character_string(victim)
		killer_string = get_character_string(killer)
		
		if victim == None or killer == None:
			print 'Somebody got killed but the program was unable to extract the victim/killer'
			return
		
		line = '%s %s (%s) was slain by %s (%s) ' % (nil.time.timestamp(), victim_name, victim_string, killer_name, killer_string)
		print line