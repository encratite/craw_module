import nil.string, utility, craw

class bncs_packet_handler_class:
	def __init__(self):
		self.whois_handler_queue = []
		
	def process_packet(self, packet):
		if len(packet) >= 2 and packet[0 : 2] == '\xff\x0f':
			account = nil.string.extract_string(packet, '(*', ')')
			if account == None:
				return
			if len(self.whois_handler_queue) > 0:
				self.whois_handler_queue[0](account)
				self.whois_handler_queue = self.whois_handler_queue[1 : ]
			
			
	def whois(self, name, handler):
		self.whois_handler_queue.append(handler)
		command = '/whois %s' % name;
		packet = '\xff\x0e' + utility.pack_number(len(command) + 5, 1) + '\x00' + command + '\x00'
		#print utility.get_packet_string(packet)
		craw.send_bncs_packet(packet)