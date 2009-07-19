import craw, string, utility, types, bind, keyboard

class command_handler_class:
	def __init__(self):
		self.town_portal_handler = None
		self.bind_handler = bind.bind_handler()
		self.keyboard_handler = keyboard.keyboard_handler_class()
		
		one_or_more = lambda x: x >= 1
		two_or_more = lambda x: x >= 2
		
		self.command_map = [
			('tp', '', 'Casts a town portal from a tome and enters it', 0, self.town_portal),
			('tppk', '', 'Casts a town portal from a tome, enters it and declares hostility to all players.', 0, self.tppk),
			('leave', '', 'Leave the current game', 0, craw.leave_game),
			('say', '<text>', 'Send a chat message to the game server', one_or_more, self.say),
			('bind', '<key> <Python expression>', 'Allows you to bind a key to an arbitrary action', two_or_more, self.bind)
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
				
			if type(argument_count) == types.FunctionType:
				argument_count_match = argument_count(len(arguments))
			else:
				argument_count_match = argument_count != len(arguments)
				
			if argument_count_match:
				print 'Invalid argument count specified.'
				print 'Usage: %s %s' % (command, arguments)
				print 'Description: %s' % description
				return True
				
			command_handler(argument_tokens)
			return True
				
		return False
			
		
	def town_portal(self, arguments):
		if self.town_portal_handler == None:
			return
			
		self.town_portal_handler.cast_town_portal()
		
	def tppk(self, arguments):
		if self.cast_town_portal == None:
			return
			
		self.town_portal_handler.town_handler = hostile.hostile_players
		self.town_portal_handler.cast_town_portal()
		
	def say(self, arguments):
		message = string.join(arguments, ' ')
		print 'Sending chat message "%s" to the server' % message
		utility.send_chat(message)
		
	def bind(self, arguments):
		key = arguments[0]
		function = string.join(arguments[1 : ], ' ')
		self.bind_handler.bind(key, function)