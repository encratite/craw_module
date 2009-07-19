class keyboard_handler_class:
	def process_key(self, key):
		key_char = chr(key)
		print 'Pressed key: %s' % key_char
		
		print 'Ignoring it'
		return
		
		try:
			keyboard_bindings[key_char]()
		except KeyError:
			pass