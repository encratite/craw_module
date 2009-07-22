import craw, threading, time, utility

class flash_thread(threading.Thread):
	def __init__(self, id, delay):
		threading.Thread.__init__(self)
		self.id = id
		self.run_thread = True
		self.delay = delay
		self.start()
		
	def run(self):
		while self.run_thread:
			flash_player(self.id)
			time.sleep(self.delay)

def flash_player(id):
	packet = '\x4b\x00\x00\x00\x00' + utility.pack_number(id, 4)
	craw.send_packet(packet)