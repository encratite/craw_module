from craw import draw_line, draw_text

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
		
		draw_line(x + start_x, y + start_y, x + end_x, y + end_y, colour)
		
def draw_automap_text(string, coordinate):
	draw_text(string, coordinate[0] - 5, coordinate[1] - 2, True)
	
def read_bytes(bytes, offset, size):
	output = 0
	for i in range(1, size + 1):
		output <<= 8
		output |= bytes[offset + size - i]
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