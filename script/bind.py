import os, utility, packets, nil.file, keyboard_configuration, craw, player_killer, random

def get_tp_handler():
	return current_handler.command_handler.town_portal_handler

def say(message):
	packets.send_chat(message)

def tp():
	get_tp_handler().cast_town_portal()
	
def town_tp():
	get_tp_handler().town_tp()
	
def tppk():
	get_tp_handler().tppk()
	
def reveal_act():
	if craw.reveal_act():
		pass
	else:
		print 'Failed to reveal the current act!'
		
def attack():
	current_handler.player_killer_handler.attack()
	
def ground_attack():
	current_handler.player_killer_handler.ground_attack()
		
def invite_all():
	print 'Inviting all players'
	my_player = utility.get_my_player()
	players = craw.get_players()
	for player in players:
		if my_player.id == player.id:
			continue
		packets.invite_player(player.id)
		
def say_file(file):
	lines = nil.file.read_lines(file)
	if lines == None:
		print 'Unable to read %s'
	elif len(lines) >= 25:
		print '%s has too many lies' % file
	else:
		print 'Reading %s and sending it to the chat' % file
		for line in lines:
			packets.send_chat(line)
			
def random_quote(directory):
	files = os.listdir(directory)
	source = files[random.randint(0, len(files) - 1)]
	say_file(os.path.join(directory, source))

current_handler = None

class bind_handler_class:
	def __init__(self):
		self.bindings = keyboard_configuration.bindings
		self.player_killer_handler = None
		craw.set_keyboard_handler(self.process_key)
		
	def bind(self, key, action):
		self.bindings[key] = action
		self.serialise_bindings()
		
	def unbind(self, key):
		self.bindings.pop(key)
		self.serialise_bindings()
		
	def print_bindings(self):
		for key in self.bindings:
			print '"%s": %s' % (key, self.bindings[key])
		
	def serialise_bindings(self):
		data = '#This file was generated automatically, do not edit it at runtime\n\n'
		data += 'bindings = {\n'
		for key in self.bindings:
			command = self.bindings[key].replace("'", "\\'")
			data += "\t'%s': '%s',\n" % (key, command)
		data += '}'
		
		data = data.replace(',\n}', '\n}')
		
		path = os.path.join(utility.get_configuration_directory(), 'keyboard_configuration.py')
		
		nil.file.write_file(path, data)
		
	def process_key(self, key):
		key_char = chr(key)
		
		try:
			global current_handler
			current_handler = self
			code = self.bindings[key_char]
			exec(code)
		except KeyError:
			pass