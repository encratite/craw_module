import craw, utility, hostile

class town_portal_class:

	def __init__(self):
		self.enter_portal = False
		self.debug = True
		self.town_handler = None
		
	def debug_print(self, message):
		if self.debug:
			print message
		
	def process_bytes(self, bytes):
		if bytes[0] != 0x82:
			return
		
		self.debug_print('Portal ownership detected: %s' % utility.get_packet_string(map(chr, bytes)))
			
		if not self.enter_portal:
			self.debug_print('Did not cast a TP')
			return
			
		if len(bytes) < 29:
			self.debug_print('The packet is too short')
			return
			
		my_id = craw.get_player_id()
		if my_id == None:
			self.debug_print('Unable to retrieve my ID')
			return
		
		player_id = utility.read_bytes(bytes, 1, 4)
		if player_id != my_id:
			self.debug_print('IDs do not match: %08x vs. %08x (%s, %s)' % (my_id, player_id, str(type(my_id)), str(type(player_id))))
			return
			
		portal_id = utility.read_bytes(bytes, 21, 4)
		
		self.enter_portal = False
		
		self.debug_print('Entering the portal')
		packet = chr(0x13) + utility.pack_number(2, 4) + utility.pack_number(portal_id, 4)
		craw.send_packet(packet)
		
		if self.town_handler != None:
			self.town_handler()
			self.town_handler = None
			
	def cast_town_portal(self):
		if utility.town_check() != False:
			print 'You cannot open a town portal inside a town.'
			return False
			
		tome_id = craw.get_tp_tome_id()
		if tome_id == None:
			print 'Unable to cast a town portal because you have no non-empty Tome of Town Portal in your inventory'
			return False
			
		location = craw.get_player_location()
		if location == None:
			print 'Unable to retrieve the location of your character'
			return False
			
		x, y = location
		
		self.enter_portal = True
			
		print 'Opening a portal'
		packet = chr(0x20) + utility.pack_number(tome_id, 4) + utility.pack_number(x, 4) + utility.pack_number(y, 4)
		craw.send_packet(packet)
		
		return True
		
	def tppk(self):
		self.town_handler = hostile.hostile_players
		self.cast_town_portal()