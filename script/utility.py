import craw, os, sys, utility

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
	
	if len(packet) == 0:
		return output
		
	if type(packet[0]) != str:
		packet = map(chr, packet)
	
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
	
def get_configuration_directory():
	return os.path.join(sys.path[-1], '..', 'configuration')
	
def read_name(bytes, offset):
	output = ''
	i = 0
	while i < 16:
		byte = bytes[offset]
		if byte == 0:
			return output
		output += chr(byte)	
		i += 1
		offset += 1
	return output
	
def get_character_class_string(character_class):
	strings = [
		'Amazon',
		'Sorceress',
		'Necromancer',
		'Paladin',
		'Barbarian',
		'Druid',
		'Assassin'
	]
	
	return strings[character_class]
	
def get_character_string(character):
	return 'Level %d %s' % (character.level, get_character_class_string(character.character_class))
	
def get_player_data_by_name(name):
	players = craw.get_players()
	for player in players:
		if player.name == name:
			return player
	return None
	
def get_my_name():
	players = craw.get_players()
	if len(players) == 0:
		return None
	player = players[0]
	return player.name
	
def get_player_by_name(name):
	players = craw.get_players()
	for player in players:
		if player.name == name:
			return player
	return None
	
def get_player_by_id(player_id):
	players = craw.get_players()
	for player in players:
		if player.id == player_id:
			return player
	return None
	
def distance(coordinate1, coordinate2):
	return ((coordinate1[0] - coordinate2[0]) ** 2 + (coordinate1[1] - coordinate2[1]) ** 2) ** 0.5
	
def get_my_player():
	players = craw.get_players()
	if players == None or len(players) == 0:
		return None
	return players[0]
	
mercenary_map = {
	0x010f: 1,
	0x0152: 2,
	0x0167: 3,
	0x0231: 5
}

def is_mercenary(id):
	return id in mercenary_map
	
def same_party(player1, player2):
	not_in_party = 0x0000ffff
	return not_in_party not in [player1.party, player2.party] and player1.party == player2.party
	
def get_party_players():
	my_player = get_my_player()
	if my_player == None:
		return None
		
	players = filter(lambda x: x.id != my_player.id and same_party(my_player, x), craw.get_players())
	return players
	
def get_d2_distance(player1, player2):
	dx = abs(player1.x - player2.x)
	dy = abs(player1.y - player2.y)
	return (dx + dy + max(dx, dy)) / 2.0
	
def get_euclidean_distance(player1, player2):
	dx = player1.x - player2.x
	dy = player1.y - player2.y
	
	return (dx**2 + dy**2)**0.5
	
def sort_units(left, right):
	difference = left[0] - right[0]
	if difference == 0:
		return 0
	return 1 if difference > 0 else -1
	
def get_my_unit():
	player = get_my_player()
	if player == None:
		return None
	return craw.get_unit(player.id, 0)