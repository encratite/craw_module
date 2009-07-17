class command_handler_class:
	def __init__(self):
		self.town_portal_handler = None
		
		self.command_map = [
			('tp', '', 'Casts a town portal', 0, self.town_portal)
		]
		
	def process_command(self, line):
		tokens = line.split(' ')
		if len(tokens) == 0:
			return False
		command = tokens[0]
		arguments = tokens[1 : ]
		
		for command_string, arguments, description, argument_count, command_handler in self.command_map:
			if command_string != command:
				continue
				
			if argument_count != len(arguments):
				print 'Invalid argument count specified.'
				print 'Usage: %s %s' % (command, arguments)
				print 'Description: %s' % description
				return True
				
			command_handler(arguments)
			return True
				
		return False
			
		
	def town_portal(self, arguments):
		if self.town_portal_handler == None:
			return
			
		self.town_portal_handler.cast_town_portal()