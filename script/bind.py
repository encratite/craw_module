import os, utility, nil.file, keyboard_configuration, craw

def get_tp_handler():
	return current_handler.command_handler.town_portal_handler

def say(message):
	utility.send_chat(message)
	
def tp():
	get_tp_handler().cast_town_portal()
	
def tppk():
	get_tp_handler().tppk()

current_handler = None

class bind_handler:
	def __init__(self):
		self.bindings = keyboard_configuration.bindings
		craw.set_keyboard_handler(self.process_key)
		
	def bind(self, key, action):
		self.bindings[key] = action
		self.serialise_bindings()
		
	def unbind(self, key):
		self.bindings.pop(key)
		self.serialise_bindings()
		
	def serialise_bindings(self):
		data = '#This file was generated automatically, do not edit it at runtime\n\n'
		data += 'bindings = {\n'
		for key in self.bindings:
			command = self.bindings[key].replace("'", "\\'")
			data += "\t'%s': '%s',\n" % (key, command)
		data += '}'
		
		path = os.path.join(utility.get_configuration_directory(), 'keyboard_configuration.py')
		
		nil.file.write_file(path, data)
		
	def process_key(self, key):
		key_char = chr(key)
		#print 'Pressed key: %s' % key_char
		
		try:
			global current_handler
			current_handler = self
			code = self.bindings[key_char]
			#print 'Executing %s' % code
			exec(code)
		except KeyError:
			pass