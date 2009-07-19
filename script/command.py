import craw, string, utility

class command_handler_class:
	def __init__(self):
		self.town_portal_handler = None
		
		self.command_map = [
			('tp', '', 'Casts a town portal from a tome and enters it', self.town_portal),
			('tppk', '', 'Casts a town portal from a tome, enters it and declares hostility to all players.', self.tppk),
			('leave', '', 'Leave the current game', craw.leave_game),
			('say', '', 'Send a chat message to the game server', self.say)
		]
		
	def process_command(self, line):
		tokens = line.split(' ')
		if len(tokens) == 0:
			return False
		command = tokens[0]
		arguments = tokens[1 : ]
		
		for command_string, arguments, description, command_handler in self.command_map:
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
		
	def tppk(self, arguments):
		if self.cast_town_portal == None:
			return
			
		self.town_portal_handler.town_handler = hostile.hostile_players
		self.town_portal_handler.cast_town_portal()
		
	def say(self, arguments):
		message = string.join(arguments, ' ')
		print 'Sending chat message "%s" to the server' % message
		utility.send_chat(message)