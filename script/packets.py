from configuration import packet_logging, packet_log
from nil.time import timestamp
from utility import get_packet_string

class packet_handler_class:

	def __init__(self):
		self.byte_handlers = []
		if packet_logging:
			self.log = open(packet_log, 'a')
		
	def add_byte_handler(self, byte_handler):
		self.byte_handlers.append(byte_handler)
		
	def process_data(self, packet):
		if packet_logging:
			self.log.write('%s %s\n' % (timestamp(), get_packet_string(packet)))
		bytes = map(ord, packet)
		for byte_handler in self.byte_handlers:
			byte_handler(bytes)
		return True