import craw, string, utility, types, bind, flash

class command_handler_class:
	def __init__(self):
		self.town_portal_handler = None
		self.skill_handler = None
		self.bind_handler = bind.bind_handler()
		self.bind_handler.command_handler = self
		self.bncs_handler = None
		self.flash_objects = []
		
		one_or_more = lambda x: x >= 1
		two_or_more = lambda x: x >= 2
		
		self.command_map = [
			('leave', '', 'Leave the current game', 0, craw.leave_game),
			('say', '<text>', 'Send a chat message to the game server', one_or_more, self.say),
			('bind', '<key> <Python expression>', 'Allows you to bind a key to an arbitrary action', two_or_more, self.bind),
			('unbind', '<key>', 'Unbinds a previously bound key', 1, self.unbind),
			('bindings', '', 'Lists all the keys currently bound', 0, self.bindings),
			('help', '', 'Prints the help for the Python commands', 0, self.print_help),
			('players', '', 'Print a list of players', 0, self.print_players),
			('skills', '', 'Sets your skills to the values currently specified', 0, self.set_skills),
			('flash', '<index> <delay in ms>', 'Flashes a player at the specified rate', 2, self.flash),
			('stop', '', 'Stop all flashing threads', 0, self.stop),
			('whois', '<player>', 'Whois a player', 1, self.whois)
		]
		
	def process_command(self, line):
		tokens = line.split(' ')
		if len(tokens) == 0:
			return False
		command = tokens[0]
		argument_tokens = tokens[1 : ]
		
		for command_string, arguments, description, argument_count, command_handler in self.command_map:
			if command_string != command:
				continue

			argument_token_count = len(argument_tokens)
				
			if type(argument_count) == types.FunctionType:
				argument_count_match = argument_count(argument_token_count)
			else:
				argument_count_match = argument_count == argument_token_count
				
			if not argument_count_match:
				print 'Invalid argument count specified.'
				print 'Usage: %s %s' % (command, arguments)
				print 'Description: %s' % description
				return True
				
			command_handler(argument_tokens)
			return True
				
		return False
		
	def set_skills(self, arguments):
		if self.skill_handler != None:
			self.skill_handler.set_skills()
		
	def say(self, arguments):
		message = string.join(arguments, ' ')
		print 'Sending chat message "%s" to the server' % message
		utility.send_chat(message)
		
	def bind(self, arguments):
		key = arguments[0]
		function = string.join(arguments[1 : ], ' ')
		self.bind_handler.bind(key, function)
		print 'Bound key "%s" to action "%s"' % (key, function)
		
	def unbind(self, arguments):
		key = arguments[0]
		self.bind_handler.unbind(key)
		print 'Unbound key "%s"' % key
		
	def bindings(self, arguments):
		print 'List of keys currently bound:'
		self.bind_handler.print_bindings()
		
	def print_help(self, arguments):
		print '\nPython commands:\n'
		for command_string, arguments, description, argument_count, command_handler in self.command_map:
			print 'Command: %s %s' % (command_string, arguments)
			print 'Description: %s\n' % description
			
	def print_players(self, arguments):
		players = craw.get_players()
		index = 0
		for player in players:
			print 'Player index: %d' % index
			print 'Name: %s' % player.name
			print 'ID: %08x' % player.id
			print 'Level: %d' % player.level
			print 'Character class: %d' % player.character_class
			print 'Life: %d' % player.life
			print 'Area: %d' % player.level_id
			print 'Location: (%d, %d)\n' % (player.x, player.y)
			index += 1
			
	def flash(self, arguments):
		try:
			index = int(arguments[0])
			delay = int(arguments[1])
		except:
			print 'Invalid input'
			return
			
		players = craw.get_players()
		if index < 0 or index >= len(players):
			print 'Invalid player index specified'
			return
			
		id = players[index].id
		print 'Launching a flash thread for id %08x at %d ms' % (id, delay)
		delay = float(delay) / 1000.0
		self.flash_objects.append(flash.flash_thread(id, delay))
		
	def stop(self, arguments):
		print 'Stopping all threads'
		for flash_object in self.flash_objects:
			flash_object.run_thread = False
		self.flash_objects = []
		
	def whois_callback(self, account):
		print 'Account: %s' % account
		
	def whois(self, arguments):
		name = arguments[0]
		print 'Running a whois on %s' % name
		self.bncs_handler.whois(name, self.whois_callback)