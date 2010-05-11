import craw, utility, packets

class town_portal_class:

	def __init__(self):
		self.debug = False
		self.tp_handler = None
		
	def debug_print(self, message):
		if self.debug:
			print message
		
	def process_bytes(self, bytes):
		ownership = packets.portal_ownership(bytes)
		if ownership == None:
			return
			
		my_id = craw.get_player_id()
		if my_id == None:
			self.debug_print('Unable to retrieve my ID')
			return
			
		player_id, portal_id = ownership
			
		if player_id != my_id:
			self.debug_print('IDs do not match: %08x vs. %08x (%s, %s)' % (my_id, player_id, str(type(my_id)), str(type(player_id))))
			return
		
		if self.tp_handler != None:
			self.tp_handler(portal_id)
			self.tp_handler = None
			
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
			
		packet = chr(0x20) + utility.pack_number(tome_id, 4) + utility.pack_number(x, 4) + utility.pack_number(y, 4)
		craw.send_packet(packet)
		
		return True
		
	def tppk(self, id = None):
		self.tp_handler = self.tppk_handler
		self.tppk_target = id
		self.cast_town_portal()
		
	def tppk_handler(self, portal_id):
		packets.enter_portal(portal_id)
		if self.tppk_target == None:
			packets.hostile_players()
		else:
			packets.hostile_player(self.tppk_target)
		
	def town_tp(self):
		self.tp_handler = self.town_tp_handler
		self.cast_town_portal()
		
	def town_tp_handler(self, portal_id):
		packets.enter_portal(portal_id)