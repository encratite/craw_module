from utility import pack_number, get_packet_string
from craw import get_player_location, get_tp_tome_id, send_packet

class command_handler_class:
	def __init__(self):
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
		tome_id = get_tp_tome_id()
		if tome_id == None:
			print 'Unable to cast a town portal because you have no non-empty Tome of Town Portal in your inventory'
			return
			
		location = get_player_location()
		if location == None:
			print 'Unable to retrieve the location of your character'
			
		x, y = location
			
		packet = '\x20' + pack_number(tome_id, 4) + pack_number(x, 4) + pack_number(y, 4)
		#print get_packet_string(packet)
		send_packet(packet)