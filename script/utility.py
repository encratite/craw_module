import craw, os, sys

def get_flag(flags, offset):
	return ((flags >> offset) & 1) != 0
	
def draw_box(coordinate, colour):
	x, y = coordinate
	
	square_size = 3
	
	coordinates = [
		(- 1, 1),
		(1, 1),
		(1, -1),
		(-1, -1),
		(-1, 1)
	]
	
	for i in range(0, len(coordinates) - 1):
		start_x, start_y = coordinates[i]
		end_x, end_y = coordinates[i + 1]
		
		start_x *= square_size
		start_y *= square_size
		
		end_x *= square_size
		end_y *= square_size
		
		craw.draw_line(x + start_x, y + start_y, x + end_x, y + end_y, colour)
		
def draw_automap_text(string, coordinate):
	craw.draw_text(string, coordinate[0] - 5, coordinate[1] - 2, True)
	
def read_bytes(bytes, offset, size):
	output = 0
	for i in range(1, size + 1):
		output <<= 8
		output |= bytes[offset + size - i]
	return output
	
def pack_number(number, size):
	output = ''
	for i in range(0, size):
		output += chr(number & 0xff)
		number >>= 8
	return output
	
def get_packet_string(packet):
	output = ''
	first = True
	for byte in packet:
		if first:
			first = False
		else:
			output += ' '
		output += '%02x' % ord(byte)
	return output
	
def town_check():
	level = craw.get_player_level()
	if level == None:
		return None
	return level in [0x01, 0x28, 0x4b, 0x67, 0x6d]
	
def send_chat(message):
	packet = '\x15\x01\x00' + message + '\x00\x00\x00'
	craw.send_packet(packet)
	
def get_configuration_directory():
	return os.path.join(sys.path[-1], '..', 'configuration')