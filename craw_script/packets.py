class packet_handler_class:

	def __init__(self):
		pass
		
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
		#print self.get_packet_string(packet)
		return True