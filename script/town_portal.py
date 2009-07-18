from craw import get_player_id, get_player_location, get_tp_tome_id, send_packet
from utility import read_bytes, pack_number, get_packet_string

class town_portal_class:

	def __init__(self):
		self.enter_portal = False
		
	def process_bytes(self, bytes):
		if not self.enter_portal or len(bytes) < 25 or bytes[0] != 0x82:
			return
			
		my_id = get_player_id()
		if my_id == None:
			return
		
		player_id = read_bytes(bytes, 1, 4)
		if player_id != my_id:
			return
			
		portal_id = read_bytes(bytes, 21, 4)
		
		self.enter_portal = False
		
		print 'Entering the portal'
		packet = chr(0x13) + pack_number(2, 4) + pack_number(portal_id, 4)
		send_packet(packet)
			
	def cast_town_portal(self):
		tome_id = get_tp_tome_id()
		if tome_id == None:
			print 'Unable to cast a town portal because you have no non-empty Tome of Town Portal in your inventory'
			return False
			
		location = get_player_location()
		if location == None:
			print 'Unable to retrieve the location of your character'
			return False
			
		x, y = location
		
		self.enter_portal = True
			
		packet = chr(0x20) + pack_number(tome_id, 4) + pack_number(x, 4) + pack_number(y, 4)
		send_packet(packet)
		
		return True