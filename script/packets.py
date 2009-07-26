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
	if bytes[0] != 0x82 or len(bytes) < 29:
		return None
	player_id = utility.read_bytes(bytes, 1, 4)
	portal_id = utility.read_bytes(bytes, 21, 4)
	return (player_id, portal_id)
	
def enter_portal(portal_id):
	packet = chr(0x13) + utility.pack_number(2, 4) + utility.pack_number(portal_id, 4)
	craw.send_packet(packet)