from configuration import packet_logging, packet_log
from nil.time import timestamp

class packet_handler_class:

	def __init__(self):
		self.byte_handlers = []
		if packet_logging:
			self.log = open(packet_log, 'a')
		
	def add_byte_handler(self, byte_handler):
		self.byte_handlers.append(byte_handler)
		
	def get_packet_string(self, packet):
		output = ''
		first = True
		for byte in packet:
			if first:
				first = False
			else:
				output += ' '
			output += '%02x' % ord(byte)
		return output
		
	def process_data(self, packet):
		#print self.get_packet_string(packet[0 : 8])
		if packet_logging:
			self.log.write('%s %s\n' % (timestamp(), self.get_packet_string(packet)))
		bytes = map(ord, packet)
		for byte_handler in self.byte_handlers:
			byte_handler(bytes)
		return True