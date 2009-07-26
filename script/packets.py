import nil.time, configuration, utility, craw

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
	
	return name, message
		
def parse_move(bytes):
	if len(bytes) < 11 or bytes[0] != 0x0f:
		return None
		
	player_id = utility.read_bytes(bytes, 2, 4)
	x = utility.read_bytes(bytes, 7, 2)
	y = utility.read_bytes(bytes, 9, 2)
	
	return player_id, x, y
	
def send_chat(message):
	packet = '\x15\x01\x00' + message + '\x00\x00\x00'
	craw.send_packet(packet)