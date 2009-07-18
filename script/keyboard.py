from configuration import keyboard_bindings

class keyboard_handler_class:
	def process_key(self, key):
		key_char = chr(key)
		#print 'Pressed key: %s' % key_char
		
		try:
			keyboard_bindings[key_char]()
		except KeyError:
			pass