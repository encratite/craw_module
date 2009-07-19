import craw, string, utility, types, bind

class command_handler_class:
	def __init__(self):
		self.town_portal_handler = None
		self.bind_handler = bind.bind_handler()
		self.bind_handler.command_handler = self
		
		one_or_more = lambda x: x >= 1
		two_or_more = lambda x: x >= 2
		
		self.command_map = [
			('leave', '', 'Leave the current game', 0, craw.leave_game),
			('say', '<text>', 'Send a chat message to the game server', one_or_more, self.say),
			('bind', '<key> <Python expression>', 'Allows you to bind a key to an arbitrary action', two_or_more, self.bind),
			('unbind', '<key>', 'Unbinds a previously bound key', 1, self.unbind),
			('help', '', 'Prints the help for the Python commands', 0, self.print_help)
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
		
	def print_help(self, arguments):
		print '\nPython commands:\n'
		for command_string, arguments, description, argument_count, command_handler in self.command_map:
			print 'Command: %s %s' % (command_string, arguments)
			print 'Description: %s\n' % description