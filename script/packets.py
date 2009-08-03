import nil.time, configuration, utility, craw, string, time

class packet_handler_class:

	def __init__(self):
		self.byte_handlers = []
		if configuration.packet_logging:
			self.log = open(configuration.packet_log, 'a')
		
	def add_byte_handler(self, byte_handler):
		self.byte_handlers.append(byte_handler)
		
	def process_data(self, packet):
		if configuration.packet_logging:
			self.log.write('[%s] %s\n' % (nil.time.timestamp(), utility.get_packet_string(packet)))
		bytes = map(ord, packet)
		for byte_handler in self.byte_handlers:
			byte_handler(bytes)
		return True
		
def portal_ownership(bytes):
	if len(bytes) < 29 or bytes[0] != 0x82:
		return None
	player_id = utility.read_bytes(bytes, 1, 4)
	portal_id = utility.read_bytes(bytes, 21, 4)
	return (player_id, portal_id)
	
def enter_portal(portal_id):
	packet = chr(0x13) + utility.pack_number(2, 4) + utility.pack_number(portal_id, 4)
	craw.send_packet(packet)
	
def parse_message(bytes):
	if len(bytes) < 14 or bytes[0 : 4] != [0x26, 0x01, 0x00, 0x02]:
		return None
	name_offset = 10
	zero_offset = name_offset
	while True:
		if bytes[zero_offset] == 0x00:
			break
		zero_offset += 1
		
	name = map(chr, bytes[name_offset : zero_offset])
	message = map(chr, bytes[zero_offset + 1 : - 1])
	
	name = string.join(name, '')
	message = string.join(message, '')
	
	return name, message
		
def parse_move(bytes):
	if len(bytes) < 11 or bytes[0] != 0x0f:
		return None
		
	player_id = utility.read_bytes(bytes, 2, 4)
	x = utility.read_bytes(bytes, 7, 2)
	y = utility.read_bytes(bytes, 9, 2)
	
	return player_id, x, y
	
def send_chat(message):
	print 'Saying "%s"' % message
	packet = '\x15\x01\x00' + message + '\x00\x00\x00'
	craw.send_packet(packet)
	
def object_assignment(bytes):
	if len(bytes) < 12 or bytes[0] != 0x51:
		return None
	object_type = bytes[1]
	object_id = utility.read_bytes(bytes, 2, 4)
	object_code = utility.read_bytes(bytes, 6, 2)
	x = utility.read_bytes(bytes, 8, 2)
	y = utility.read_bytes(bytes, 10, 2)
	return object_type, object_id, object_code, x, y
	
def town_portal_assignment(bytes):
	assignment = object_assignment(bytes)
	if assignment == None:
		return None
		
	object_type, object_id, object_code, x, y = assignment
	if object_type != 0x02 or object_code != 0x003b:
		return None
		
	return object_id, x, y
	
def object_removal(bytes):
	if len(bytes) != 6 or bytes[0] != 0x0a:
		return None
		
	type = bytes[1]
	id = utility.read_bytes(bytes, 2, 4)
	
	return type, id
	
def set_right_skill(skill):
	packet = '\x3c' + utility.pack_number(skill, 2) + '\x00\x00\xff\xff\xff\xff'
	craw.send_packet(packet)
	
def cast_right_skill_at_target(type, target):
	packet = '\x0d' + utility.pack_number(type, 4) + utility.pack_number(target, 4)
	craw.send_packet(packet)
	
def cast_right_skill_at_location(x, y):
	packet = '\x0c' + utility.pack_number(x, 2) +utility.pack_number(y, 2)
	craw.send_packet(packet)
	
def cast_right_skill():
	player = utility.get_my_player()
	cast_right_skill_at_location(player.x, player.y)
	
def perform_cast(skill, switch_delay, cast_delay):
	set_right_skill(skill)
	time.sleep(switch_delay)
	cast_right_skill()
	time.sleep(cast_delay)

def assign_mercenary(bytes):
	if len(bytes) != 20 or bytes[0] != 0x81:
		return None
		
	type = utility.read_bytes(bytes, 2, 2)
	try:
		mercenary_act = utility.mercenary_map[type]
	except KeyError:
		mercenary_act = None
	
	owner_id = utility.read_bytes(bytes, 4, 4)
	mercenary_id = utility.read_bytes(bytes, 8, 4)
	
	return mercenary_act, owner_id, mercenary_id
	
def accept_invitation(player_id):
	packet = '\x5e\x08' + utility.pack_number(player_id, 4)
	craw.send_packet(packet)
	
def grant_loot_permission(player_id):
	packet = '\x5d\x01\x01' + utility.pack_number(player_id, 4)
	craw.send_packet(packet)

def invite_player(player_id):
	packet = '\x5e\x06' + utility.pack_number(player_id, 4)
	craw.send_packet(packet)
	
def parse_invitiation(bytes):
	if len(bytes) < 6 or bytes[0] != 0x8b or bytes[5] != 0x02:
		return None
		
	player_id = utility.read_bytes(bytes, 1, 4)
	return player_id

def parse_join(bytes):
	if len(bytes) < 26 or bytes[0] != 0x5b:
		return None
		
	player_id = utility.read_bytes(bytes, 3, 4)
	player_name = utility.read_name(bytes, 8)
	character_class = bytes[7]
	level = utility.read_bytes(bytes, 24, 2)
	
	return player_id, player_name, character_class, level
	
def parse_assignment(bytes):
	if len(bytes) < 26 or bytes[0] != 0x59:
		return None
		
	player_id = utility.read_bytes(bytes, 1, 4)
	character_class = bytes[5]
	player_name = utility.read_name(bytes, 6)
	x = utility.read_bytes(bytes, 22, 2)
	y = utility.read_bytes(bytes, 24, 2)
	return player_id, character_class, player_name, x, y
	
def entering_game(bytes):
	return bytes[0] == 0x01
	
def load_complete(bytes):
	return bytes[0] == 0x04