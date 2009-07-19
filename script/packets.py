import nil.time, configuration, utility

class packet_handler_class:

	def __init__(self):
		self.byte_handlers = []
		if configuration.packet_logging:
			self.log = open(packet_log, 'a')
		
	def add_byte_handler(self, byte_handler):
		self.byte_handlers.append(byte_handler)
		
	def process_data(self, packet):
		if configuration.packet_logging:
			self.log.write('%s %s\n' % (nil.time.timestamp(), utility.get_packet_string(packet)))
		bytes = map(ord, packet)
		for byte_handler in self.byte_handlers:
			byte_handler(bytes)
		return True