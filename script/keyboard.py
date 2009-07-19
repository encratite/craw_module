import craw

class keyboard_handler_class:
	def __init__(self):
		craw.set_keyboard_handler(self.process_key)
		
	def process_key(self, key):
		key_char = chr(key)
		print 'Pressed key: %s' % key_char
		
		print 'Ignoring it'
		return
		
		try:
			keyboard_bindings[key_char]()
		except KeyError:
			pass