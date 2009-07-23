import nil.string, utility, craw, threading

class bncs_packet_handler_class:
	def __init__(self):
		self.queue_lock = threading.Lock()
		self.whois_handler_queue = []
		
	def process_packet(self, packet):
		if len(packet) >= 2 and packet[0 : 2] == '\xff\x0f':
			account = nil.string.extract_string(packet, '(*', ')')
			if account == None:
				return
			print 'Account: %s' % account
			self.queue_lock.acquire()
			if len(self.whois_handler_queue) > 0:
				self.whois_handler_queue[0](account)
				self.whois_handler_queue = self.whois_handler_queue[1 : ]
			self.queue_lock.release()
			
			
	def whois(self, name, handler):
		self.queue_lock.acquire()
		self.whois_handler_queue.append(handler)
		command = '/whois %s' % name;
		packet = '\xff\x0e' + utility.pack_number(len(command) + 5, 1) + '\x00' + command + '\x00'
		print utility.get_packet_string(packet)
		craw.send_bncs_packet(packet)
		self.queue_lock.release()