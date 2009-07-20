import craw, utility

def hostile_player(id):
	packet = '\x5d\x04\x01' + utility.pack_number(id, 4)
	print utility.get_packet_string(packet)
	return packet
	
def hostile_players():
	print 'Declaring hostility to all players'
	player_ids = craw.get_player_ids()
	my_id = craw.get_player_id()
	player_ids = filter(lambda id: id != my_id, player_ids)
	packet = ''
	for id in player_ids:
		packet += hostile_player(id)
	craw.send_packet(packet)