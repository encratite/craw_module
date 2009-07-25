class item_handler_class:
	def __init__(self):
		self.is_drop = False
		
	def process_bytes(self, bytes):
		if len(bytes) < 2:
			return
			
		self.is_drop = (bytes[0 : 2] == [0x9c, 0x00])
				
	def process_item(self, item):
		if self.is_drop:
			self.process_drop(item)
			
	def print_item(self, item):
		print 'Name: %s' % item.type
		print 'Code: %s' % item.code
		print 'ID: %08x' % item.id
		print 'Quality: %d' % item.quality
		print 'Level: %d' % item.level
		print 'Sockets: %d' % item.sockets
	
	def process_drop(self, item):
		pass